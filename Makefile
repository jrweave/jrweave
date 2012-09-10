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

include Makefile.inc

# When adding a new subdirectory, make sure to modify DIRS
DIRS      = test sys util ex ptr ucs iri lang rdf rif io
EXE       = exec
OBJS      =
OBJLIBS		=
LIBS      = -L.

all : build $(EXE)

thorough : runtests
	$(ECHO) building with thorough tests
	-for d in $(DIRS); do ($(ECHO) cd $$d; cd $$d; $(ECHO) $(MAKE) $(MFLAGS) thorough; $(MAKE) $(MFLAGS) thorough); done

runtests : all force_look
	$(ECHO) running tests
	-for d in $(DIRS); do ($(ECHO) cd $$d; cd $$d; $(ECHO) $(MAKE) $(MFLAGS) runtests; $(MAKE) $(MFLAGS) runtests); done

build : prep
	$(ECHO) building
	-for d in $(DIRS); do ($(ECHO) cd $$d; cd $$d; $(ECHO) $(MAKE) $(MFLAGS) build; $(MAKE) $(MFLAGS) build); done

prep :
	$(ECHO) preparing build
	-for d in $(DIRS); do ($(ECHO) cd $$d; cd $$d; $(ECHO) $(MAKE) $(MFLAGS) prep; $(MAKE) $(MFLAGS) prep); done

clean :
	$(ECHO) cleaning up
	-$(RM) -vf $(EXE) $(OBJS) $(OBJLIBS)
	-$(RM) -vfr *.dSYM
	-for d in $(DIRS); do ($(ECHO) cd $$d; cd $$d; $(ECHO) $(MAKE) $(MFLAGS) clean; $(MAKE) $(MFLAGS) clean); done

force_look :
	true

# add main.o to dependencies
$(EXE) : $(OBJLIBS)
	$(ECHO) This is where compiling exec would happen
#$(ECHO) $(LD) -o $(EXE) $(OBJS) $(LIBS)
#$(LD) -o $(EXE) $(OBJS) $(LIBS)

test : force_look
	$(ECHO) looking into test : $(MAKE) $(MFLAGS)
	cd test; $(MAKE) $(MFLAGS)

sys : test force_look
	$(ECHO) looking into sys : $(MAKE) $(MFLAGS)
	cd sys; $(MAKE) $(MFLAGS)

util : sys force_look
	$(ECHO) looking into util : $(MAKE) $(MFLAGS)
	cd util; $(MAKE) $(MFLAGS)

ex : util force_look
	$(ECHO) looking into ex : $(MAKE) $(MFLAGS)
	cd ex; $(MAKE) $(MFLAGS)

ptr : ex force_look
	$(ECHO) looking into ptr : $(MAKE) $(MFLAGS)
	cd ptr; $(MAKE) $(MFLAGS)

ucs : ptr force_look
	$(ECHO) looking into ucs : $(MAKE) $(MFLAGS)
	cd ucs; $(MAKE) $(MFLAGS)

iri : ucs force_look
	$(ECHO) looking into iri : $(MAKE) $(MFLAGS)
	cd iri; $(MAKE) $(MFLAGS)

lang : ptr force_look
	$(ECHO) looking into lang : $(MAKE) $(MFLAGS)
	cd lang; $(MAKE) $(MFLAGS)

rdf : iri lang force_look
	$(ECHO) looking into rdf : $(MAKE) $(MFLAGS)
	cd rdf; $(MAKE) $(MFLAGS)

rif : iri force_look
	$(ECHO) looking into rif : $(MAKE) $(MFLAGS)
	cd rif; $(MAKE) $(MFLAGS)

io : ptr force_look
	$(ECHO) looking into io : $(MAKE) $(MFLAGS)
	cd io; $(MAKE) $(MFLAGS)
