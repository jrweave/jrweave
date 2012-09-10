#!/usr/bin/perl

# Copyright 2012 Jesse Weaver
# 
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
# 
#        http://www.apache.org/licenses/LICENSE-2.0
# 
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
#    implied. See the License for the specific language governing
#    permissions and limitations under the License.


# It's a mess, but it works.

sub print_hash {
	my $hashref = $_[0];
	my $prefix = $_[1];
	foreach my $key (keys(%{$hashref})) {
		print $prefix . ": " . $key . " -> " . $hashref->{$key} . "\n";
	}
}

sub generate_map {
	# %records referenced as global variable
	my $type = $_[0];
	my $field = $_[1];
	my $check = $_[2];
	my $map = {};
	foreach my $name (keys(%{$records{$type}})) {
		if (exists($records{$type}{$name}{$field})
				&& exists($records{$type}{$name}{$check})
				&& $name ne $records{$type}{$name}{$field}) {
			$map->{$name} = $records{$type}{$name}{$field};
		}
	}
	return $map;
}

sub unpack_char {
	my $n = $_[0];
	if ($n == 0) {
		return "\0";
	} elsif ($n < 10 + 1) {
		my @zero = unpack("C*", "0");
		return pack("C*", ($n - 1 + $zero[0]));
	} elsif ($n < 37 + 1) {
		my @a = unpack("C*", "A");
		return pack("C*", ($n - 1 - 10 + $a[0]));
	} elsif ($n == 37 + 1) {
		return "-";
	}
	print STDERR "CANNOT UNPACK CHAR " . $n . " !\n";
}

sub pack_char {
	my $c = uc(substr($_[0], 0, 1));
	if ($c ge "0" && $c le "9") {
		my @zero = unpack("C*", "0");
		my @char = unpack("C*", $c);
		return $char[0] - $zero[0] + 1;
	} elsif ($c ge "A" && $c le "Z") {
		my @a = unpack("C*", "A");
		my @char = unpack("C*", $c);
		return $char[0] - $a[0] + 10 + 1;
	} elsif ($c eq "-") {
		return 37 + 1;
	}
	print STDERR "CANNOT PACK CHAR " . $c . " !\n";
	exit(1);
}

sub unpack_str {
	my $arref = $_[0];
	my $str = "";
	while (scalar(@$arref) > 0) {
		my $word = shift(@$arref);
		$word = ($word >> 2);
		my $sub = "";
		for (my $i = 0; $i < 5 && ($word > 0 || scalar(@$arref) > 0); $i++) {
			$c = unpack_char($word & 0x3F);
			if ($c ne "\0") {
				$sub = $c . $sub;
				$word = ($word >> 6);
			}
		}
		$str .= $sub;
	}
	return $str;
}

sub pack_str {
	my $str = uc($_[0]);
	my $index = [];
	for (my $i = 0; $i < length($str); $i += 5) {
		my $len = length($str) - $i;
		if ($len > 5) {
			$len = 5;
		}
		my $sub = substr($str, $i, $len);
		my $idx = 0;
		for (my $j = 0; $j < $len; $j++) {
			$idx = ($idx << 6) | pack_char(substr($sub, $j, 1));
		}
		$idx = ($idx << 2);
		push(@$index, $idx);
	}
	return $index;
}

sub generate_index {
	my $map = generate_map($_[0], $_[1], $_[2]);
	my $index = {};
	foreach my $name (keys(%$map)) {
		my @keys = @{pack_str($name)};
		my $hashref = $index;
		foreach my $key (@keys) {
			if (!exists($hashref->{$key})) {
				$hashref->{$key} = {};
			}
			$hashref = $hashref->{$key};
		}
		$hashref->{1} = $name; # leaf
		$hashref->{2} = $map->{$name}; # leaf
	}
	return $index;
}

sub print_c_index {
	my $idxref = $_[0];
	my $prefix = $_[1];
	if (scalar(keys(%$idxref)) <= 0) {
		print STDERR "extern const void **" . $prefix . "_VALUES;\n";
		print "const void **" . $prefix . "_VALUES = NULL;\n";
		return;
	}
	my $retcode = 0x2;
	if (exists($idxref->{1})) {
		if (!exists($strings{$idxref->{2}})) {
			my $strname = "LANG_STRING_" . uc($idxref->{2});
			$strname =~ s/-/_/g;
			$strings{$idxref->{2}} = $strname;
			$strname =~ s/-/_/g;
			print STDERR "extern const uint8_t " . $strname . "[];\n";

			print "const uint8_t " . $strname . "[" . (length($idxref->{2}) + 1) . "] = {\n";
			print "\tUINT8_C(" . length($idxref->{2}) . "), // length\n";
			my @ascii = unpack("C*", $idxref->{2});
			foreach my $c (@ascii) {
				print "\tUINT8_C(" . $c . "),\n";
			}
			print "};\n";
			print "\n";
		}
		print "// " . $idxref->{1} . " -> " . $idxref->{2} . "\n";
		print "#define " . $prefix . "_LEAF " . $strings{$idxref->{2}} . "\n";
		print "\n";
		delete $idxref->{1};
		delete $idxref->{2};
		$retcode = $retcode & 0xC;
	}
	if (scalar(%$idxref) <= 0) {
		return $retcode;
	}
	$retcode = $retcode | 0x1;
	my %codes;
	foreach my $key (sort{$a<=>$b}keys(%$idxref)) {
		my $new_prefix = $prefix . "_" . sprintf("%08X", $key);
		my $code = print_c_index($idxref->{$key}, $new_prefix);
		$codes{$key} = $code;
	}

	print STDERR "extern const uint32_t " . $prefix . "_KEYS[];\n";
	print STDERR "extern const void *" . $prefix . "_VALUES[];\n";
	
	print "const uint32_t " . $prefix . "_KEYS[" . (scalar(keys(%$idxref)) + 1) . "] = {\n";
	print "\tUINT32_C(" . scalar(keys(%$idxref)) . "), // length\n";
	foreach my $key (sort{$a<=>$b}keys(%$idxref)) {
		my $code = $codes{$key};
		if (($code & 0x2) == 0) {
			my $hex = sprintf("%08X", $key);
			print "\tUINT32_C(0x" . $hex . "), // " . unpack_str([$key]) . "\n";
		}
		if (($code & 0x1) != 0) {
			my $hex = sprintf("%08X", ($key | 0x1));
			print "\tUINT32_C(0x" . $hex . "), // " . unpack_str([$key]) . "\n";
		}
	}
	print "};\n";
	print "\n";
	print "const void *" . $prefix . "_VALUES[" . (scalar(keys(%$idxref)) + 1) . "] = {\n";
	print "\t" . $prefix . "_KEYS, // keys for _this_ array\n";
	foreach my $key (sort{$a<=>$b}keys(%$idxref)) {
		my $code = $codes{$key};
		if (($code & 0x2) == 0) {
			my $hex = sprintf("%08X", $key);
			print "\t" . $prefix . "_" . $hex . "_LEAF, // " . unpack_str([$key]) . "\n";
		}
		if (($code & 0x1) != 0) {
			my $hex = sprintf("%08X", $key);
			print "\t" . $prefix . "_" . $hex . "_VALUES, // " . unpack_str([$key]) . "\n";
		}
	}
	print "};\n";
	print "\n";
	return $retcode;
}

sub print_c_array {
	my $type = $_[0];
	my $field = $_[1];
	my $tag = $_[2];
	my $indexref = generate_index($type, $field, $tag);
	my $prefix = uc($field);
	$prefix =~ s/-/_/g;
	$prefix = "LANG_" . uc($type) . "_" . uc($tag) . "_" . $prefix;
	print_c_index($indexref, $prefix);
}

%strings = ();
$last_key = "";
open REG, "<language-subtag-registry";
while (<REG>) {
	if ($_ =~ m/^%%$/) {
		if (scalar(keys(%fields)) > 0) {
			$name = exists($fields{"Tag"}) ? $fields{"Tag"} : $fields{"Subtag"};
			foreach $key (keys(%fields)) {
				$records{$fields{"Type"}}{$name}{$key} = $fields{$key};
			}
		}
		foreach $key (keys %fields) {
			delete $fields{$key};
		}
	} elsif ($_ =~ m/^([^\s]+): (.+)$/) {
		$fields{$1} = $2;
		$last_key = $1;
	} else {
		$fields{$last_key} .= $_;
	}
}
close REG;

print STDERR "#define LANG_CHAR_ENCODE(c) (((c) == UINT8_C(0x2D) ? UINT8_C(37) : ((c) <= UINT8_C(0x39) ? ((c) - UINT8_C(0x30)) : ((c) < UINT8_C(0x61) ? ((c) - UINT8_C(0x41) + 10) : ((c) - UINT8_C(0x61) + 10)))) + UINT8_C(1))\n";
print STDERR "\n";

print_c_array("extlang", "Prefix", "Subtag");

print_c_array("grandfathered", "Preferred-Value", "Tag");
print_c_array("grandfathered", "Preferred-Value", "Subtag");
print_c_array("redundant", "Preferred-Value", "Tag");
print_c_array("redundant", "Preferred-Value", "Subtag");
print_c_array("language", "Preferred-Value", "Subtag");
print_c_array("script", "Preferred-Value", "Subtag");
print_c_array("region", "Preferred-Value", "Subtag");
print_c_array("variant", "Preferred-Value", "Subtag");
print_c_array("extlang", "Preferred-Value", "Subtag");
