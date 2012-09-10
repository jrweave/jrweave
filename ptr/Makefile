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

SUBDIR	= ptr
CFLAGS  = $(PRJCFLAGS) -I..
OBJS		= alloc.o BadAllocException.o Ptr.o SizeUnknownException.o

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

alloc.o : alloc.h alloc-inl.h
	$(ECHO) $(CC) $(CFLAGS) -c -o alloc.o alloc.cpp
	$(CC) $(CFLAGS) -c -o alloc.o alloc.cpp

SizeUnknownException.o : ../ex/TraceableException.h SizeUnknownException.h SizeUnknownException.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o SizeUnknownException.o SizeUnknownException.cpp
	$(CC) $(CFLAGS) -c -o SizeUnknownException.o SizeUnknownException.cpp

BadAllocException.o : ../ex/TraceableException.h BadAllocException.h BadAllocException.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o BadAllocException.o BadAllocException.cpp
	$(CC) $(CFLAGS) -c -o BadAllocException.o BadAllocException.cpp

Ptr.o : BadAllocException.o Ptr.h Ptr.cpp
	$(ECHO) $(CC) $(CFLAGS) -c -o Ptr.o Ptr.cpp
	$(CC) $(CFLAGS) -c -o Ptr.o Ptr.cpp
