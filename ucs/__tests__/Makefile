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

SUBDIR		= ucs/__tests__
CFLAGS		= $(PRJCFLAGS) -I../..
TESTS			= testnf testutf
THOROUGH	= testnf_thoroughly

all :

clean :
	$(ECHO) cleaning up in $(SUBDIR)
	$(ECHO) -$(RM) -f $(TESTS)
	-$(RM) -vf $(TESTS)
	-$(RM) -vf $(THOROUGH)
	-$(RM) -vfr *.dSYM

thorough : runtests $(THOROUGH)

runtests : $(TESTS)

force_look :
	true

testutf : testutf.cpp ../utf.h ../utf.o ../UTF8Iter.o ../UTF16Iter.o ../UTF32Iter.o
	$(ECHO) running test $(SUBDIR)/testutf
	$(ECHO) $(CC) $(CFLAGS) -o testutf testutf.cpp ../utf.o ../../sys/endian.o ../../ptr/SizeUnknownException.o. ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../UTF8Iter.o ../UTF16Iter.o ../UTF32Iter.o ../../ptr/alloc.o ../nf.o
	$(CC) $(CFLAGS) -o testutf testutf.cpp ../utf.o ../../sys/endian.o ../../ptr/SizeUnknownException.o ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../UTF8Iter.o ../UTF16Iter.o ../UTF32Iter.o ../../ptr/alloc.o ../nf.o
	$(ECHO) [TEST] ./testutf
	./testutf

testnf : testnf.cpp ../nf.h ../nf.o
	$(ECHO) running test $(SUBDIR)/testnf
	$(ECHO) $(CC) $(CFLAGS) -I.. -c -o __nf.o nf.cpp
	cd ..; $(CC) $(CFLAGS) -I.. -c -o __nf.o nf.cpp; cd -
	$(ECHO) $(CC) $(CFLAGS) -o testnf testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o. ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(CC) $(CFLAGS) -o testnf testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(ECHO) [TEST] ./testnf
	./testnf
	-$(RM) -vf ../__nf.o
	$(ECHO) $(CC) $(CFLAGS) -o testnf testnf.cpp ../nf.o ../../ptr/SizeUnknownException.o. ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(CC) $(CFLAGS) -o testnf testnf.cpp ../nf.o ../../ptr/SizeUnknownException.o ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o

testnf_thoroughly :
	$(ECHO) running test $(SUBDIR)/testnf_thoroughly with -DUCS_NO_K
	$(ECHO) $(CC) $(CFLAGS) -I.. -DUCS_NO_K -c -o __nf.o nf.cpp
	cd ..; $(CC) $(CFLAGS) -I.. -DUCS_NO_K -c -o __nf.o nf.cpp; cd -
	$(ECHO) $(CC) $(CFLAGS) -DUCS_NO_K -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o. ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(CC) $(CFLAGS) -DUCS_NO_K -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(ECHO) [TEST] ./testnf_thoroughly
	./testnf_thoroughly
	$(ECHO) running test $(SUBDIR)/testnf_thoroughly with -DUCS_NO_C
	$(ECHO) $(CC) $(CFLAGS) -I.. -DUCS_NO_C -c -o __nf.o nf.cpp
	cd ..; $(CC) $(CFLAGS) -I.. -DUCS_NO_C -c -o __nf.o nf.cpp; cd -
	$(ECHO) $(CC) $(CFLAGS) -DUCS_NO_C -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o. ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(CC) $(CFLAGS) -DUCS_NO_C -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(ECHO) [TEST] ./testnf_thoroughly
	./testnf_thoroughly
	$(ECHO) running test $(SUBDIR)/testnf_thoroughly with -DUCS_NO_K -DUCS_NO_C
	$(ECHO) $(CC) $(CFLAGS) -I.. -DUCS_NO_K -DUCS_NO_C -c -o __nf.o nf.cpp
	cd ..; $(CC) $(CFLAGS) -I.. -DUCS_NO_K -DUCS_NO_C -c -o __nf.o nf.cpp; cd -
	$(ECHO) $(CC) $(CFLAGS) -DUCS_NO_K -DUCS_NO_C -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o. ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(CC) $(CFLAGS) -DUCS_NO_K -DUCS_NO_C -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(ECHO) [TEST] ./testnf_thoroughly
	./testnf_thoroughly
	$(ECHO) running test $(SUBDIR)/testnf_thoroughly with -DUCS_TRUST_CODEPOINTS
	$(ECHO) $(CC) $(CFLAGS) -I.. -DUCS_TRUST_CODEPOINTS -c -o __nf.o nf.cpp
	cd ..; $(CC) $(CFLAGS) -I.. -DUCS_TRUST_CODEPOINTS -c -o __nf.o nf.cpp; cd -
	$(ECHO) $(CC) $(CFLAGS) -DUCS_TRUST_CODEPOINTS -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o. ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(CC) $(CFLAGS) -DUCS_TRUST_CODEPOINTS -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(ECHO) [TEST] ./testnf_thoroughly
	./testnf_thoroughly
	$(ECHO) running test $(SUBDIR)/testnf_thoroughly with -DUCS_PLAY_DUMB
	$(ECHO) $(CC) $(CFLAGS) -I.. -DUCS_PLAY_DUMB -c -o __nf.o nf.cpp
	cd ..; $(CC) $(CFLAGS) -I.. -DUCS_PLAY_DUMB -c -o __nf.o nf.cpp; cd -
	$(ECHO) $(CC) $(CFLAGS) -DUCS_PLAY_DUMB -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o. ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(CC) $(CFLAGS) -DUCS_PLAY_DUMB -o testnf_thoroughly testnf.cpp ../__nf.o ../../ptr/SizeUnknownException.o ../../ptr/Ptr.o ../../ex/TraceableException.o ../../ptr/BadAllocException.o ../InvalidEncodingException.o ../InvalidCodepointException.o ../../ptr/alloc.o
	$(ECHO) [TEST] ./testnf_thoroughly
	./testnf_thoroughly
	-$(RM) -vf ../__nf.o
