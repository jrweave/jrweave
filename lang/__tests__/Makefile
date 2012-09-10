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

include ../../Makefile.inc

SUBDIR	= lang/__tests__
CFLAGS	= $(PRJCFLAGS) -I../..
TESTS		= testLangTag testLangRange

all :

clean :
	$(ECHO) cleaning up in $(SUBDIR)
	$(ECHO) -$(RM) -f $(TESTS)
	-$(RM) -vf $(TESTS)
	-$(RM) -vfr *.dSYM

thorough : runtests

runtests : $(TESTS)

force_look :
	true

testLangTag : testLangTag.cpp ../LangTag.o
	$(ECHO) running test $(SUBDIR)/testLangTag
	$(ECHO) $(CC) $(CFLAGS) -o testLangTag testLangTag.cpp ../LangTag.o ../../ptr/SizeUnknownException.o ../MalformedLangTagException.o ../../ex/TraceableException.o ../../ptr/Ptr.o ../../ptr/BadAllocException.o ../../ptr/alloc.o ../../sys/char.o ../../ucs/UTF8Iter.o ../../ucs/utf.o ../../ucs/InvalidCodepointException.o ../../ucs/InvalidEncodingException.o ../../ucs/nf.o
	$(CC) $(CFLAGS) -o testLangTag testLangTag.cpp ../LangTag.o ../../ptr/SizeUnknownException.o ../MalformedLangTagException.o ../../ex/TraceableException.o ../../ptr/Ptr.o ../../ptr/BadAllocException.o ../../ptr/alloc.o ../../sys/char.o ../../ucs/UTF8Iter.o ../../ucs/utf.o ../../ucs/InvalidCodepointException.o ../../ucs/InvalidEncodingException.o ../../ucs/nf.o
	$(ECHO) [TEST] ./testLangTag
	./testLangTag

testLangRange : testLangRange.cpp ../LangRange.o ../LangTag.o
	$(ECHO) running test $(SUBDIR)/testLangRange
	$(ECHO) $(CC) $(CFLAGS) -o testLangRange testLangRange.cpp ../LangRange.o ../LangTag.o ../../ptr/SizeUnknownException.o ../MalformedLangTagException.o ../MalformedLangRangeException.o ../../ex/TraceableException.o ../../ptr/Ptr.o ../../ptr/BadAllocException.o ../../ptr/alloc.o ../../sys/char.o ../../ucs/utf.o ../../ucs/InvalidCodepointException.o ../../ucs/InvalidEncodingException.o ../../ucs/nf.o
	$(CC) $(CFLAGS) -o testLangRange testLangRange.cpp ../LangRange.o ../LangTag.o ../../ptr/SizeUnknownException.o ../MalformedLangTagException.o ../MalformedLangRangeException.o ../../ex/TraceableException.o ../../ptr/Ptr.o ../../ptr/BadAllocException.o ../../ptr/alloc.o ../../sys/char.o ../../ucs/utf.o ../../ucs/InvalidCodepointException.o ../../ucs/InvalidEncodingException.o ../../ucs/nf.o
	$(ECHO) [TEST] ./testLangRange
	./testLangRange
