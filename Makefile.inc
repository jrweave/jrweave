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

CC        = mpicxx
#PRJCFLAGS = -O3 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DSYSTEM=SYS_DEFAULT -DMPICH_IGNORE_CXX_SEEK -DTIMING_USE=3 -DPTR_MEMDEBUG
#PRJCFLAGS = -O3 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DSYSTEM=SYS_DEFAULT -DMPICH_IGNORE_CXX_SEEK -DTIMING_USE=3 -DUCS_TRUST_CODEPOINTS -DUCS_PLAY_DUMB
PRJCFLAGS = -O3 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DSYSTEM=SYS_DEFAULT -DMPICH_IGNORE_CXX_SEEK -DTIMING_USE=0
LD        = g++
LDFLAGS   =
AR        = ar
ARFLAGS   =
RANLIB    = ranlib
RM        = rm
ECHO      = echo
RUN				= mpirun
TESTFILE	= /Users/jesseweaver/Desktop/_test_


EX_OBJS		= ../ex/TraceableException.o
IO_OBJS		= ../io/IOException.o ../io/InputStream.o ../io/OutputStream.o
IRI_OBJS		= ../iri/MalformedIRIRefException.o ../iri/IRIRef.o
LANG_OBJS		= ../lang/MalformedLangTagException.o ../lang/LangTag.o ../lang/MalformedLangRangeException.o ../lang/LangRange.o
PTR_OBJS		= ../ptr/alloc.o ../ptr/BadAllocException.o ../ptr/Ptr.o ../ptr/SizeUnknownException.o
RDF_OBJS		= ../rdf/RDFTerm.o ../rdf/RDFTriple.o ../rdf/NTriplesReader.o ../rdf/NTriplesWriter.o
RIF_OBJS		= ../rif/RIFConst.o ../rif/RIFVar.o ../rif/RIFTerm.o ../rif/RIFAtomic.o ../rif/RIFCondition.o ../rif/RIFDictionary.o
SYS_OBJS		= ../sys/endian.o ../sys/char.o
UCS_OBJS		= ../ucs/InvalidCodepointException.o ../ucs/InvalidEncodingException.o ../ucs/nf.o ../ucs/utf.o ../ucs/UTF8Iter.o ../ucs/UTF16Iter.o ../ucs/UTF32Iter.o
UTIL_OBJS		=

SHELL     = /bin/sh

.SILENT :
