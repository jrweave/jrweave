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

SUBDIR	= iri/__tests__
CFLAGS	= $(PRJCFLAGS) -I../..
TESTS		= testIRIRef

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

testIRIRef : testIRIRef.cpp ../IRIRef.o
	$(ECHO) running test $(SUBDIR)/testIRIRef
	$(ECHO) $(CC) $(CFLAGS) -o testIRIRef testIRIRef.cpp ../IRIRef.o ../../ptr/SizeUnknownException.o ../MalformedIRIRefException.o ../../ex/TraceableException.o ../../ptr/Ptr.o ../../ptr/BadAllocException.o ../../ucs/UTF8Iter.o ../../ucs/nf.o ../../ucs/utf.o ../../ucs/InvalidCodepointException.o ../../ucs/InvalidEncodingException.o ../../sys/endian.o ../../ptr/alloc.o ../../sys/char.o
	$(CC) $(CFLAGS) -o testIRIRef testIRIRef.cpp ../IRIRef.o ../../ptr/SizeUnknownException.o ../MalformedIRIRefException.o ../../ex/TraceableException.o ../../ptr/Ptr.o ../../ptr/BadAllocException.o ../../ucs/UTF8Iter.o ../../ucs/nf.o ../../ucs/utf.o ../../ucs/InvalidCodepointException.o ../../ucs/InvalidEncodingException.o ../../sys/endian.o ../../ptr/alloc.o ../../sys/char.o
	$(ECHO) [TEST] ./testIRIRef
	./testIRIRef
