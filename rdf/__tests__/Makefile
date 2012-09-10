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

SUBDIR	= rdf/__tests__
CFLAGS	= $(PRJCFLAGS) -I../..
TESTS		= testRDFTerm testRDFDictionary testNTriplesReader testNTriplesWriter

all :

clean :
	$(ECHO) cleaning up in $(SUBDIR)
	$(ECHO) -$(RM) -f $(TESTS)
	-$(RM) -vf $(TESTS)
	-$(RM) -vfr *.dSYM
	-$(RM) -vf foaf.out

thorough : runtests

runtests : $(TESTS)

force_look :
	true

testRDFTerm : testRDFTerm.cpp ../RDFTerm.o
	$(ECHO) running test $(SUBDIR)/testRDFTerm
	$(ECHO) $(CC) $(CFLAGS) -o testRDFTerm testRDFTerm.cpp ../RDFTerm.o ../../iri/IRIRef.o ../../lang/LangTag.o ../../ptr/Ptr.o ../../ptr/SizeUnknownException.o ../../ex/TraceableException.o ../../ucs/utf.o ../../ucs/nf.o ../../lang/MalformedLangTagException.o ../../iri/MalformedIRIRefException.o ../../ucs/InvalidEncodingException.o ../../ptr/BadAllocException.o ../../sys/endian.o ../../ucs/InvalidCodepointException.o ../../ptr/alloc.o ../../ucs/UTF8Iter.o
	$(CC) $(CFLAGS) -o testRDFTerm testRDFTerm.cpp ../RDFTerm.o ../../iri/IRIRef.o ../../lang/LangTag.o ../../ptr/Ptr.o ../../ptr/SizeUnknownException.o ../../ex/TraceableException.o ../../ucs/utf.o ../../ucs/nf.o ../../lang/MalformedLangTagException.o ../../iri/MalformedIRIRefException.o ../../ucs/InvalidEncodingException.o ../../ptr/BadAllocException.o ../../sys/endian.o ../../ucs/InvalidCodepointException.o ../../ptr/alloc.o ../../ucs/UTF8Iter.o
	$(ECHO) [TEST] ./testRDFTerm
	./testRDFTerm

testRDFDictionary : testRDFDictionary.cpp ../RDFDictionary.h ../RDFDictionary-inl.h
	$(ECHO) running test $(SUBDIR)/testRDFDictionary
	$(ECHO) $(CC) $(CFLAGS) -o testRDFDictionary testRDFDictionary.cpp ../RDFTerm.o ../../iri/IRIRef.o ../../lang/LangTag.o ../../ptr/Ptr.o ../../ptr/SizeUnknownException.o ../../ex/TraceableException.o ../../ucs/utf.o ../../ucs/nf.o ../../lang/MalformedLangTagException.o ../../iri/MalformedIRIRefException.o ../../ucs/InvalidEncodingException.o ../../ptr/BadAllocException.o ../../sys/endian.o ../../ucs/InvalidCodepointException.o ../../ptr/alloc.o ../../ucs/UTF8Iter.o
	$(CC) $(CFLAGS) -o testRDFDictionary testRDFDictionary.cpp ../RDFTerm.o ../../iri/IRIRef.o ../../lang/LangTag.o ../../ptr/Ptr.o ../../ptr/SizeUnknownException.o ../../ex/TraceableException.o ../../ucs/utf.o ../../ucs/nf.o ../../lang/MalformedLangTagException.o ../../iri/MalformedIRIRefException.o ../../ucs/InvalidEncodingException.o ../../ptr/BadAllocException.o ../../sys/endian.o ../../ucs/InvalidCodepointException.o ../../ptr/alloc.o ../../ucs/UTF8Iter.o
	$(ECHO) [TEST] ./testRDFDictionary
	./testRDFDictionary

testNTriplesReader : testNTriplesReader.cpp ../NTriplesReader.h foaf.nt
	$(ECHO) running test $(SUBDIR)/testNTriplesReader
	$(ECHO) $(CC) $(CFLAGS) -o testNTriplesReader testNTriplesReader.cpp ../NTriplesReader.o  ../RDFTriple.o ../RDFTerm.o ../../iri/IRIRef.o ../../lang/LangTag.o ../../ptr/Ptr.o ../../ptr/SizeUnknownException.o ../../ex/TraceableException.o ../../ucs/utf.o ../../ucs/nf.o ../../lang/MalformedLangTagException.o ../../iri/MalformedIRIRefException.o ../../ucs/InvalidEncodingException.o ../../ptr/BadAllocException.o ../../sys/endian.o ../../ucs/InvalidCodepointException.o ../../ptr/alloc.o ../../ucs/UTF8Iter.o ../../io/InputStream.o ../../io/IOException.o
	$(CC) $(CFLAGS) -o testNTriplesReader testNTriplesReader.cpp ../NTriplesReader.o ../RDFTriple.o ../RDFTerm.o ../../iri/IRIRef.o ../../lang/LangTag.o ../../ptr/Ptr.o ../../ptr/SizeUnknownException.o ../../ex/TraceableException.o ../../ucs/utf.o ../../ucs/nf.o ../../lang/MalformedLangTagException.o ../../iri/MalformedIRIRefException.o ../../ucs/InvalidEncodingException.o ../../ptr/BadAllocException.o ../../sys/endian.o ../../ucs/InvalidCodepointException.o ../../ptr/alloc.o ../../ucs/UTF8Iter.o ../../io/InputStream.o ../../io/IOException.o
	$(ECHO) [TEST] ./testNTriplesReader
	./testNTriplesReader

testNTriplesWriter : testNTriplesWriter.cpp ../NTriplesWriter.h ../NTriplesWriter.cpp foaf.nt
	-$(RM) -vf foaf.out
	$(ECHO) running test $(SUBDIR)/testNTriplesWriter
	$(ECHO) $(CC) $(CFLAGS) -o testNTriplesWriter testNTriplesWriter.cpp ../NTriplesWriter.o ../NTriplesReader.o ../RDFTriple.o ../RDFTerm.o ../../iri/IRIRef.o ../../lang/LangTag.o ../../ptr/Ptr.o ../../ptr/SizeUnknownException.o ../../ex/TraceableException.o ../../ucs/utf.o ../../ucs/nf.o ../../lang/MalformedLangTagException.o ../../iri/MalformedIRIRefException.o ../../ucs/InvalidEncodingException.o ../../ptr/BadAllocException.o ../../sys/endian.o ../../ucs/InvalidCodepointException.o ../../ptr/alloc.o ../../ucs/UTF8Iter.o ../../io/IOException.o ../../io/InputStream.o ../../io/OutputStream.o
	$(CC) $(CFLAGS) -o testNTriplesWriter testNTriplesWriter.cpp ../NTriplesWriter.o ../NTriplesReader.o ../RDFTriple.o ../RDFTerm.o ../../iri/IRIRef.o ../../lang/LangTag.o ../../ptr/Ptr.o ../../ptr/SizeUnknownException.o ../../ex/TraceableException.o ../../ucs/utf.o ../../ucs/nf.o ../../lang/MalformedLangTagException.o ../../iri/MalformedIRIRefException.o ../../ucs/InvalidEncodingException.o ../../ptr/BadAllocException.o ../../sys/endian.o ../../ucs/InvalidCodepointException.o ../../ptr/alloc.o ../../ucs/UTF8Iter.o ../../io/IOException.o ../../io/InputStream.o ../../io/OutputStream.o
	$(ECHO) [TEST] ./testNTriplesWriter
	./testNTriplesWriter
