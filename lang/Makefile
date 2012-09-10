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

include ../Makefile.inc

SUBDIR	= lang
CFLAGS  = $(PRJCFLAGS) -I.. -I/usr/include
OBJS		= MalformedLangTagException.o LangTag.o MalformedLangRangeException.o LangRange.o

all : build __tests__

thorough : runtests
	cd __tests__; $(MAKE) $(MFLAGS) thorough

build : prep $(OBJS)

prep : lang_arrays.h lang_arrays.cpp

__tests__ : force_look
	$(ECHO) looking into $(SUBDIR)/__tests__ : $(MAKE) $(MFLAGS)
	cd __tests__; $(MAKE) $(MFLAGS)

runtests : all
	$(ECHO) running tests for $(SUBDIR) : $(MAKE) $(MFLAGS) runtests
	cd __tests__; $(MAKE) $(MFLAGS) runtests

clean :
	$(ECHO) cleaning up in $(SUBDIR)
	-$(RM) -vf $(OBJS) 
	-$(RM) -vfr *.dSYM
	-$(RM) -vf lang_arrays.*
	cd __tests__; $(MAKE) $(MFLAGS) clean

force_look :
	true

lang_arrays.h : lang_arrays.cpp

lang_arrays.cpp : scripts/index.pl scripts/language-subtag-registry
	$(ECHO) generating lang_arrays.h and lang_arrays.cpp
	cd scripts; perl index.pl 2> ../lang_arrays.h > ../lang_arrays.cpp

MalformedLangTagException.o : MalformedLangTagException.h MalformedLangTagException.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o MalformedLangTagException.o MalformedLangTagException.cpp
	$(CC) $(CFLAGS) -c -o MalformedLangTagException.o MalformedLangTagException.cpp

LangTag.o : LangTag.h LangTag-inl.h LangTag.cpp lang_arrays.h lang_arrays.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o LangTag.o LangTag.cpp
	$(CC) $(CFLAGS) -c -o LangTag.o LangTag.cpp

MalformedLangRangeException.o : MalformedLangRangeException.h MalformedLangRangeException.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o MalformedLangRangeException.o MalformedLangRangeException.cpp
	$(CC) $(CFLAGS) -c -o MalformedLangRangeException.o MalformedLangRangeException.cpp

LangRange.o : LangRange.h LangRange-inl.h LangRange.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o LangRange.o LangRange.cpp
	$(CC) $(CFLAGS) -c -o LangRange.o LangRange.cpp
