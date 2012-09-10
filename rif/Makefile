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

SUBDIR	= rif
CFLAGS  = $(PRJCFLAGS) -I.. -I/usr/include
OBJS		= RIFConst.o RIFVar.o RIFTerm.o RIFAtomic.o RIFCondition.o RIFDictionary.o

all : build __tests__

thorough : runtests
	cd __tests__; $(MAKE) $(MFLAGS) thorough

build : prep $(OBJS)

prep : 

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
	cd __tests__; $(MAKE) $(MFLAGS) clean

force_look :
	true

RIFConst.o : RIFConst.h RIFConst-inl.h RIFConst.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o RIFConst.o RIFConst.cpp
	$(CC) $(CFLAGS) -c -o RIFConst.o RIFConst.cpp

RIFVar.o : RIFVar.h RIFVar-inl.h RIFVar.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o RIFVar.o RIFVar.cpp
	$(CC) $(CFLAGS) -c -o RIFVar.o RIFVar.cpp

RIFTerm.o : RIFTerm.h RIFTerm-inl.h RIFTerm.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o RIFTerm.o RIFTerm.cpp
	$(CC) $(CFLAGS) -c -o RIFTerm.o RIFTerm.cpp

RIFAtomic.o : RIFAtomic.h RIFAtomic-inl.h RIFAtomic.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o RIFAtomic.o RIFAtomic.cpp
	$(CC) $(CFLAGS) -c -o RIFAtomic.o RIFAtomic.cpp

RIFCondition.o : RIFCondition.h RIFCondition-inl.h RIFCondition.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o RIFCondition.o RIFCondition.cpp
	$(CC) $(CFLAGS) -c -o RIFCondition.o RIFCondition.cpp

RIFDictionary.o : RIFDictionary.h RIFDictionary-inl.h RIFDictionary.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o RIFDictionary.o RIFDictionary.cpp
	$(CC) $(CFLAGS) -c -o RIFDictionary.o RIFDictionary.cpp
