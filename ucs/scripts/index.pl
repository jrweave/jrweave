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

$CODE_VALUE = 0;
$CHAR_NAME = 1;
$CANON_COMB_CLASS = 3;
$DECOMP_MAPPING = 5;
$UNICODE_1_0_NAME = 10;

$SBASE = hex('AC00');
$LBASE = hex('1100');
$VBASE = hex('1161');
$TBASE = hex('11A7');
$SCOUNT = 11172;
$LCOUNT = 19;
$VCOUNT = 21;
$TCOUNT = 28;
$NCOUNT = $VCOUNT * $TCOUNT;

sub decpaircmp {
	my ($a, $b) = @_;
	@as = split(/ /, $a);
	@bs = split(/ /, $b);
	if ($as[0] < $bs[0]) {
		return -1;
	} elsif ($as[0] > $bs[0]) {
		return 1;
	} elsif ($as[1] < $bs[1]) {
		return -1;
	} elsif ($as[1] > $bs[1]) {
		return 1;
	}
	return 0;
}

sub decompose {
	my $map = $_[0];
	my @string = @{$_[1]};
	my @nfd = ();
	foreach (@string) {
		push(@nfd, @{$map->[hex($_)]});
	}
	return \@nfd;
}

sub order {
	# @data referenced as global variable
	my @nfd = @{$_[0]};
	for ($i = 1; $i < scalar(@nfd); $i++) {
		if ($data[$nfd[$i-1]]{'ccc'} > $data[$nfd[$i]]{'ccc'} && $data[$nfd[$i]]{'ccc'} > 0) {
			$swap = $nfd[$i-1];
			$nfd[$i-1] = $nfd[$i];
			$nfd[$i] = $swap;
			if ($i > 1) {
				$i -= 2;
			}
		}
	}
	return \@nfd;
}

sub compose {
	# @data referenced as global variable
	# %comp_pairs referenced as global variable
	my @nfd = @{$_[0]};
	my @nfc = ($nfd[0]);
	for ($i = 1; $i < scalar(@nfd); $i++) {
		$c = $nfd[$i];
		$starter = undef;
		$starti = 0;
		for ($j = $#nfc; $j >= 0; $j--) {
			if ($data[$nfc[$j]]{'ccc'} == 0) {
				$starter = $nfc[$j];
				$starti = $j;
				last;
			}
			if ($data[$nfc[$j]]{'ccc'} >= $data[$c]{'ccc'}){
				last;
			}
		}
		if ($starter == undef) {
			push(@nfc, $c);
			next;
		}
		if (exists($comp_pairs{"$starter $c"})) {
			$nfc[$starti] = $comp_pairs{"$starter $c"};
		} else {
			push(@nfc, $c);
		}
	}
	return \@nfc;
}

sub verify {
	@dec = @{$_[0]};
	@hex = @{$_[1]};
	$test = $_[2];
 	if (scalar(@dec) != scalar(@hex)) {
		print "FAILED $test " . $line;
		print "EXPECTED @hex\n";
		print "FOUND ";
		foreach (@dec) {
			printf("%X ", $_);
		}
		print "\n";
		return 1;
	} else {
		for ($i = 0; $i < scalar(@dec); $i++) {
			if ($dec[$i] != hex($hex[$i])) {
				print "FAILED $test " . $line;
				print "EXPECTED @hex\n";
				print "FOUND ";
				foreach (@dec) {
					printf("%X ", $_);
				}
				print "\n";
				return 1;
			}
		}
	}
	return 0;
}

$data = ();
$last_codepoint = 0;
open DATA, "<UnicodeData.txt";
while (<DATA>) {
	@chardata = split(/;/, $_);
	$codepoint = hex($chardata[$CODE_VALUE]);
	if ($codepoint > $last_codepoint + 1) {
		if ($chardata[$CHAR_NAME] =~ m/^<(.*), Last>$/ || $codepoint == hex('AC00')) {
			if ($codepoint == hex('AC00') || $codepoint == hex('D7A3')) {
				$adjust = 1;
			} else {
				$adjust = 0;
			}
			for ($last_codepoint++; $last_codepoint < $codepoint + $adjust; $last_codepoint++) {
				$data[$last_codepoint] = {
					'name' => "<$1>",
					'ccc' => 0,
					'decomp' => [ $last_codepoint ],
				};
				if (hex('AC00') <= $last_codepoint && $last_codepoint <= hex('D7A3')) {
					$sindex = $last_codepoint - $SBASE;
					$l = $LBASE + int($sindex / $NCOUNT);
					$v = $VBASE + int(($sindex % $NCOUNT) / $TCOUNT);
					$t = $TBASE + $sindex % $TCOUNT;
					if ($t == $TBASE) {
						@d = ($l, $v);
					} else {
						$lv = (($l - $LBASE) * $VCOUNT + ($v - $VBASE)) * $TCOUNT + $SBASE;
						@d = ($lv, $t);
					}
					$data[$last_codepoint]{'decomp'} = [ @d ];
				}
			}
			if ($adjust == 1) {
				$last_codepoint = $codepoint;
				next;
			}
		} else {
			for ($last_codepoint++; $last_codepoint < $codepoint; $last_codepoint++) {
				$data[$last_codepoint] = undef;
			}
		}
	}
	$data[$codepoint] = {
		'name' => $chardata[$CHAR_NAME] . ';' . $chardata[$UNICODE_1_0_NAME],
		'ccc' => $chardata[$CANON_COMB_CLASS] eq "" ? 0 : $chardata[$CANON_COMB_CLASS],
	};
	$dmap = $chardata[$DECOMP_MAPPING];
	if ($dmap =~ m/^<(.*)> /) {
		$data[$codepoint]{'decompflag'} = $1;
		$dmap = substr($dmap, length($1) + 3);
	}
	if ($dmap ne "") {
		@decomp = ();
		@d = split(/ /, $dmap);
		foreach (@d) {
			push(@decomp, hex($_));
		}
		$data[$codepoint]{'decomp'} = [ @decomp ];
	} else {
		$data[$codepoint]{'decomp'} = [ $codepoint ];
	}
	$last_codepoint = $codepoint;
}
close DATA;

open HANGUL, "<HangulSyllableType.txt";
while (<HANGUL>) {
	if ($_ =~ m/^([0-9A-F]+)((?:\.\.[0-9A-F]+)?)\s+;\s+([A-Z]+)/) {
		$max = $2 eq "" ? $1 : substr($2, 2);
		for ($i = hex($1); $i <= hex($max); $i++) {
			$data[$i]{'hangul'} = $3;
		}
	}
}
close HANGUL;

open EXCLUDE, "<DerivedNormalizationProps.txt";
while (<EXCLUDE>) {
	if ($_ =~ m/^([0-9A-F]+)((?:\.\.[0-9A-F]+)?)\s*;\sFull_Composition_Exclusion\s/) {
		$max = $2 eq "" ? $1 : substr($2, 2);
		for ($i = hex($1); $i <= hex($max); $i++) {
			$data[$i]{'exclude'} = 1;
		}
	} elsif ($_ =~ m/^([0-9A-F]+)((?:\.\.[0-9A-F]+)?)\s*;\s+(NFK?[CD]_QC);\s+([YNM])\s/) {
		if ($4 ne "Y") {
			$max = $2 eq "" ? $1 : substr($2, 2);
			for ($i = hex($1); $i <= hex($max); $i++) {
				$data[$i]{lc($3)} = $4;
			}
		}
	}
}
close EXCLUDE;

for ($codepoint = 0; $codepoint < scalar(@data); $codepoint++) {
	if ($data[$codepoint] == undef) {
		next;
	}
	if (exists($data[$codepoint]{'decompflag'})) {
		next;
	}
	@decomp = @{$data[$codepoint]{'decomp'}};
	if (scalar(@decomp) == 2
			&& (!exists($data[$codepoint]{'hangul'})
					|| $data[$codepoint]{'hangul'} =~ m/^LVT?$/)
			&& !exists($data[$codepoint]{'exclude'})) {
		$comp_pairs{"@decomp"} = $codepoint;
	}
}

for ($codepoint = 0; $codepoint < scalar(@data); $codepoint++) {
	if ($data[$codepoint] == undef) {
		$testd[$codepoint] = undef;
		$testkd[$codepoint] = undef;
		next;
	}

	if (exists($data[$codepoint]{'decompflag'})) {
		@newdecomp = ($codepoint);
	} else {
		@newdecomp = @{$data[$codepoint]{'decomp'}};
	}
	do {
		@decomp = @newdecomp;
		@newdecomp = ();
		foreach (@decomp) {
			if (exists($data[$_]{'decompflag'})) {
				@subdecomp = ($_);
			} else {
				@subdecomp = @{$data[$_]{'decomp'}};
			}
			push(@newdecomp, @subdecomp);
		}
	} while ("@decomp" ne "@newdecomp");
	$testd[$codepoint] = [ @decomp ];

	@newdecomp = @{$data[$codepoint]{'decomp'}};
	do {
		@decomp = @newdecomp;
		@newdecomp = ();
		foreach (@decomp) {
			@subdecomp = @{$data[$_]{'decomp'}};
			push(@newdecomp, @subdecomp);
		}
	} while ("@decomp" ne "@newdecomp");
	$testkd[$codepoint] = [ @decomp ];
}

open TEST, "<NormalizationTest.txt";
$tests = 0;
$failures = 0;
while (<TEST>) {
	$line = $_;
	if ($line =~ m/([0-9A-F]+(?: [0-9A-F]+)*);([0-9A-F]+(?: [0-9A-F]+)*);([0-9A-F]+(?: [0-9A-F]+)*);([0-9A-F]+(?: [0-9A-F]+)*);([0-9A-F]+(?: [0-9A-F]+)*);/) {
		@string = split(/ /, $1);
		@nfc_expected = split(/ /, $2);
		@nfd_expected = split(/ /, $3);
		@nfkc_expected = split(/ /, $4);
		@nfkd_expected = split(/ /, $5);

		$ref = decompose(\@testd, \@string);
		@nfd = @{$ref};
		$ref = order(\@nfd);
		@nfd = @{$ref};
		$ref = compose(\@nfd);
		@nfc = @{$ref};

		$ref = decompose(\@testkd, \@string);
		@nfkd = @{$ref};
		$ref = order(\@nfkd);
		@nfkd = @{$ref};
		$ref = compose(\@nfkd);
		@nfkc = @{$ref};

		$failures += verify(\@nfd, \@nfd_expected, 'NFD');
		$failures += verify(\@nfc, \@nfc_expected, 'NFC');
		$failures += verify(\@nfkd, \@nfkd_expected, 'NFKD');
		$failures += verify(\@nfkc, \@nfkc_expected, 'NFKC');
	}
	$tests += 4;
}
close TEST;

if ($failures > 0) {
	print "FAILED $failures OUT OF $tests\n";
	exit(1);
}

# PREPARE OUTPUT DATA

@ranges = (0);
$last = 1;
for ($i = 1; $i < scalar(@data); $i++) {
	if ($data[$i] == undef && $last != undef) {
		push(@ranges, $i);
		$last = undef;
	} elsif ($data[$i] != undef && $last == undef) {
		push(@ranges, $i);
		$last = $i;
	}
}
if ($last != undef) {
	push (@ranges, $i);
}

@kdranges = ();
@kdindex = ();
$kdcount = 0;
$last = undef;
for ($i = 0; $i < scalar(@data); $i++) {
	@decomp = ($testkd[$i] == undef) ? () : @{$testkd[$i]};
	$valid = $data[$i] != undef &&
			(("@decomp" ne "$i" && "@decomp" ne "") ||
			 ($data[$i]{'ccc'} != 0 && $data[$i]{'ccc'} ne "") ||
			 (exists($data[$i]{'nfd_qc'}) && $data[$i]{'nfd_qc'} ne "Y") ||
			 (exists($data[$i]{'nfkd_qc'}) && $data[$i]{'nfkd_qc'} ne "Y") ||
			 (exists($data[$i]{'nfc_qc'}) && $data[$i]{'nfc_qc'} ne "Y") ||
			 (exists($data[$i]{'nfkc_qc'}) && $data[$i]{'nfkc_qc'} ne "Y"));
	if ($valid) {
		if ($last == undef) {
			push(@kdranges, $i);
			push(@kdindex, $kdcount);
			$last = $i;
		}
		$kdcount++;
	} else {
		if ($last != undef) {
			push(@kdranges, $i);
			$last = undef;
		}
	}
}
if ($last != undef) {
	push(@kdranges, $i);
}

@dranges = ();
@dindex = ();
$dcount = 0;
$last = undef;
for ($i = 0; $i < scalar(@data); $i++) {
	@decomp = ($testd[$i] == undef) ? () : @{$testd[$i]};
	$valid = $data[$i] != undef &&
			(("@decomp" ne "$i" && "@decomp" ne "") ||
			 ($data[$i]{'ccc'} != 0 && $data[$i]{'ccc'} ne ""));
	if ($valid) {
		if ($last == undef) {
			push(@dranges, $i);
			push(@dindex, $dcount);
			$last = $i;
		}
		$dcount++;
	} else {
		if ($last != undef) {
			push(@dranges, $i);
			$last = undef;
		}
	}
}
if ($last != undef) {
	push(@dranges, $i);
}

@keys = sort{decpaircmp($a,$b)}(keys(%comp_pairs));
@cranges = ($keys[0]);
@cindex = (0);
$ccount = 1;
@last = split(' ', $keys[0]);
for ($i = 1; $i < scalar(@keys); $i++) {
	@key = split(' ', $keys[$i]);
	if ($key[0] > $last[0] || ($key[0] == $last[0] && $key[1] > $last[1] + 1)) {
		push(@cranges, $last[0] . " " . ($last[1] + 1), $keys[$i]);
		push(@cindex, $ccount);
		@last = @key;
	}
	$ccount++;
}
@last = split(' ' , $keys[$#keys]);
push(@cranges, $last[0] . " " . ($last[1] + 1));

# .h to STDOUT
# .cpp to STDERR

# DECOMP FORMAT: array of uint32_t, one header, rest codepoints
# HEADER FORMAT: | 8 : ccc | 8 : qcs | 8 : compatibility length | 8 : canonical length |
# QCS FORMAT: | 2 : NFD_QC | 2 : NFKD_QC | 2 : NFC_QC | 2 :NFKC_QC |
#			where each 2-bit value is one of 10 = MAYBE, 00 = NO, 01 = YES
# CODEPOINT FORMAT: | 8 : ccc | 24 : codepoint |

%qc_map = ( "N" => "UCS_QC_NO", "Y" => "UCS_QC_YES", "M" => "UCS_QC_MAYBE" );

print "#include \"sys/ints.h\"\n";
print "\n";
print "#define UCS_QC_NO UINT8_C(0x00)\n";
print "#define UCS_QC_YES UINT8_C(0x01)\n";
print "#define UCS_QC_MAYBE UINT8_C(0x02)\n";
print "\n";
print "#define UCS_DECOMP_HEADER(ccc, nfdqc, nfkdqc, nfcqc, nfkcqc, compat_len, canon_len) ( \\\n";
print "\t(((uint32_t)(ccc) & UINT32_C(0x0FF)) << 24) | \\\n";
print "\t(((uint32_t)(nfdqc) & UINT32_C(0x3)) << 22) | \\\n";
print "\t(((uint32_t)(nfkdqc) & UINT32_C(0x3)) << 20) | \\\n";
print "\t(((uint32_t)(nfcqc) & UINT32_C(0x3)) << 18) | \\\n";
print "\t(((uint32_t)(nfkcqc) & UINT32_C(0x3)) << 16) | \\\n";
print "\t(((uint32_t)(compat_len) & UINT32_C(0x0FF)) << 8) | \\\n";
print "\t((uint32_t)(canon_len) & UINT32_C(0x0FF)) \\\n";
print ")\n";
print "#define UCS_DECOMP_CCC(ptr) (*((uint32_t*)(ptr)) >> 24)\n";
print "#define UCS_DECOMP_NFD_QC(ptr) ((*((uint32_t*)(ptr)) >> 22) & UINT32_C(0x3))\n";
print "#define UCS_DECOMP_NFKD_QC(ptr) ((*((uint32_t*)(ptr)) >> 20) & UINT32_C(0x3))\n";
print "#define UCS_DECOMP_NFC_QC(ptr) ((*((uint32_t*)(ptr)) >> 18) & UINT32_C(0x3))\n";
print "#define UCS_DECOMP_NFKC_QC(ptr) ((*((uint32_t*)(ptr)) >> 16) & UINT32_C(0x3))\n";
print "#define UCS_DECOMP_CANON_LEN(ptr) (*((uint32_t*)(ptr)) & UINT32_C(0x0FF))\n";
print "#define UCS_DECOMP_CANON_CHARS(ptr) (((uint32_t*)(ptr)) + 1)\n";
print "#define UCS_DECOMP_COMPAT_LEN(ptr) (((*((uint32_t*)(ptr)) >> 8) & UINT32_C(0x0FF)) == UINT32_C(0) ? UCS_DECOMP_CANON_LEN(ptr) : ((*((uint32_t*)(ptr)) >> 8) & UINT32_C(0x0FF)))\n";
print "#define UCS_DECOMP_COMPAT_CHARS(ptr) (((*((uint32_t*)(ptr)) >> 8) & UINT32_C(0x0FF)) == UINT32_C(0) ? UCS_DECOMP_CANON_CHARS(ptr) : (UCS_DECOMP_CANON_CHARS(ptr) + UCS_DECOMP_CANON_LEN(ptr)))\n";
print "#define UCS_PACK(ccc, cp) ((((uint32_t)(ccc)) << 24) | ((uint32_t)(cp)))\n";
print "#define UCS_UNPACK_CCC(packed) (((uint32_t)(packed)) >> 24)\n";
print "#define UCS_UNPACK_CODEPOINT(packed) (((uint32_t)(packed)) & UINT32_C(0x00FFFFFF))\n";

print "#ifndef UCS_PLAY_DUMB\n";
print "\n";

print STDERR "#include \"ucs/ucs_arrays.h\"\n";
print STDERR "\n";
print STDERR "#ifndef UCS_PLAY_DUMB\n";
print STDERR "\n";

# CODEPOINT VALIDATION

print "#ifndef UCS_TRUST_CODEPOINTS\n";
print "\n";
print "extern const uint32_t UCS_CODEPOINT_RANGES_LEN;\n";
print "extern const uint32_t UCS_CODEPOINT_RANGES[];\n";
print "\n";
print "#endif /* !defined(UCS_TRUST_CODEPOINTS) */\n";
print "\n";

print STDERR "#ifndef UCS_TRUST_CODEPOINTS\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_CODEPOINT_RANGES_LEN = UINT32_C(" . scalar(@ranges) . ");\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_CODEPOINT_RANGES[" . scalar(@ranges) . "] = {\n";
foreach (@ranges) {
	printf(STDERR "\tUINT32_C(0x%04X),\n", $_);
}
print STDERR "};\n";
print STDERR "\n";
print STDERR "#endif /* !defined(UCS_TRUST_CODEPOINTS) */\n";
print STDERR "\n";

# DECOMPOSITION

print "extern const uint32_t UCS_DECOMPOSITION_RANGES_LEN;\n";
print "extern const uint32_t UCS_DECOMPOSITION_RANGES[];\n";
print "extern const uint32_t UCS_DECOMPOSITION_OFFSETS[];\n";
print "extern const uint32_t *UCS_DECOMPOSITIONS[];\n";
print "\n";

print STDERR "#ifdef UCS_NO_K\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_DECOMPOSITION_RANGES_LEN = UINT32_C(" . scalar(@dranges) . ");\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_DECOMPOSITION_RANGES[" . scalar(@dranges) . "] = {\n";
foreach (@dranges) {
	printf(STDERR "\tUINT32_C(0x%04X),\n", $_);
}
print STDERR "};\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_DECOMPOSITION_OFFSETS[" . scalar(@dindex) . "] = {\n";
foreach (@dindex) {
	print STDERR "\t$_,\n";
}
print STDERR "};\n";
print STDERR "\n";
for ($i = 0; $i < scalar(@dranges); $i += 2) {
	for ($j = $dranges[$i]; $j < $dranges[$i+1]; $j++) {
		@decomp = @{$testd[$j]};
		printf(STDERR "const uint32_t UCS_%04X_DECOMPOSITION[%d] = {\n", $j, scalar(@decomp) + 1);
		print STDERR "\tUCS_DECOMP_HEADER(" . $data[$j]{'ccc'} . ", ";
		print STDERR $qc_map{(!exists($data[$j]{'nfd_qc'}) ? "Y" : $data[$j]{'nfd_qc'})} . ", ";
		print STDERR $qc_map{(!exists($data[$j]{'nfkd_qc'}) ? "Y" : $data[$j]{'nfkd_qc'})} . ", ";
		print STDERR $qc_map{(!exists($data[$j]{'nfc_qc'}) ? "Y" : $data[$j]{'nfc_qc'})} . ", ";
		print STDERR $qc_map{(!exists($data[$j]{'nfkc_qc'}) ? "Y" : $data[$j]{'nfkc_qc'})} . ", ";
		print STDERR "0, " . scalar(@decomp) . "),\n";
		foreach (@decomp) {
			printf(STDERR "\tUCS_PACK(%d, 0x%04X),\n", $data[$_]{'ccc'}, $_);
		}
		print STDERR "};\n";
		print STDERR "\n";
	}
}
print STDERR "const uint32_t *UCS_DECOMPOSITIONS[$dcount] = {\n";
for ($i = 0; $i < scalar(@dranges); $i += 2) {
	for ($j = $dranges[$i]; $j < $dranges[$i+1]; $j++) {
		printf(STDERR "\tUCS_%04X_DECOMPOSITION,\n", $j);
	}
}
print STDERR "};\n";
print STDERR "\n";
print STDERR "#else /* !defined(UCS_NO_K) */\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_DECOMPOSITION_RANGES_LEN = UINT32_C(" . scalar(@kdranges) . ");\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_DECOMPOSITION_RANGES[" . scalar(@kdranges) . "] = {\n";
foreach (@kdranges) {
	printf(STDERR "\tUINT32_C(0x%04X),\n", $_);
}
print STDERR "};\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_DECOMPOSITION_OFFSETS[" . scalar(@kdindex) . "] = {\n";
foreach (@kdindex) {
	print STDERR "\t$_,\n";
}
print STDERR "};\n";
print STDERR "\n";
for ($i = 0; $i < scalar(@kdranges); $i += 2) {
	for ($j = $kdranges[$i]; $j < $kdranges[$i+1]; $j++) {
		@d = @{$testd[$j]};
		@decomp = @{$testkd[$j]};
		$compatlen = "@d" eq "@decomp" ? 0 : scalar(@decomp);
		printf(STDERR "const uint32_t UCS_%04X_DECOMPOSITION[%d] = {\n", $j, scalar(@d) + $compatlen + 1);
		print STDERR "\tUCS_DECOMP_HEADER(" . $data[$j]{'ccc'} . ", ";
		print STDERR $qc_map{(!exists($data[$j]{'nfd_qc'}) ? "Y" : $data[$j]{'nfd_qc'})} . ", ";
		print STDERR $qc_map{(!exists($data[$j]{'nfkd_qc'}) ? "Y" : $data[$j]{'nfkd_qc'})} . ", ";
		print STDERR $qc_map{(!exists($data[$j]{'nfc_qc'}) ? "Y" : $data[$j]{'nfc_qc'})} . ", ";
		print STDERR $qc_map{(!exists($data[$j]{'nfkc_qc'}) ? "Y" : $data[$j]{'nfkc_qc'})} . ", ";
		print STDERR $compatlen . ", " . scalar(@d) . "),\n";
		foreach (@d) {
			printf(STDERR "\tUCS_PACK(%d, 0x%04X),\n", $data[$_]{'ccc'}, $_);
		}
		if ($compatlen > 0) {
			foreach (@decomp) {
				printf(STDERR "\tUCS_PACK(%d, 0x%04X),\n", $data[$_]{'ccc'}, $_);
			}
		}
		print STDERR "};\n";
		print STDERR "\n";
	}
}
print STDERR "const uint32_t *UCS_DECOMPOSITIONS[$kdcount] = {\n";
for ($i = 0; $i < scalar(@kdranges); $i += 2) {
	for ($j = $kdranges[$i]; $j < $kdranges[$i+1]; $j++) {
		printf(STDERR "\tUCS_%04X_DECOMPOSITION,\n", $j);
	}
}
print STDERR "};\n";
print STDERR "\n";
print STDERR "#endif /* UCS_NO_K */\n";
print STDERR "\n";

# COMPOSITION

print "#ifndef UCS_NO_C\n";
print "\n";
print "extern const uint32_t UCS_COMPOSITION_INDEX_LEN;\n";
print "extern const uint64_t UCS_COMPOSITION_INDEX[];\n";
print "extern const uint32_t UCS_COMPOSITIONS[];\n";
print "\n";
print "#endif /* UCS_NO_C */\n";
print "\n";

print STDERR "#ifndef UCS_NO_C\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_COMPOSITION_INDEX_LEN = UINT32_C(" . scalar(@keys) . ");\n";
print STDERR "\n";
print STDERR "const uint64_t UCS_COMPOSITION_INDEX[" . scalar(@keys) . "] = {\n";
foreach (@keys) {
	@pair = split(' ', $_);
	printf(STDERR "\tUINT64_C(0x%04X%08X),\n", $pair[0], $pair[1]);
}
print STDERR "};\n";
print STDERR "\n";
print STDERR "const uint32_t UCS_COMPOSITIONS[" . scalar(@keys) . "] = {\n";
foreach (@keys) {
	printf(STDERR "\tUINT32_C(0x%04X),\n", $comp_pairs{$_});
}
print STDERR "};\n";
print STDERR "\n";
print STDERR "#endif /* !defined(UCS_NO_C) */\n";
print STDERR "\n";

# END

print "#endif /* !defined(UCS_PLAY_DUMB) */\n";

print STDERR "#endif /* !defined(UCS_PLAY_DUMB) */\n";
