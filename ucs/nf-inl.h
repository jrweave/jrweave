/* Copyright 2012 Jesse Weaver
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *    implied. See the License for the specific language governing
 *    permissions and limitations under the License.
 */

#include "ucs/nf.h"

namespace ucs {

using namespace ex;
using namespace ptr;
using namespace std;

#if !defined(UCS_PLAY_DUMB)
inline
bool nfcmpccc(const uint32_t a, const uint32_t b) throw() {
  return UCS_UNPACK_CCC(a) < UCS_UNPACK_CCC(b);
}
#endif

#if defined(UCS_TRUST_CODEPOINTS) || defined(UCS_PLAY_DUMB)
inline
bool nfvalid(const uint32_t codepoint) throw() {
  return true;
}
#endif

#if defined(UCS_TRUST_CODEPOINTS) || defined(UCS_PLAY_DUMB)
inline
bool nfvalid(DPtr<uint32_t> *codepoints) throw(SizeUnknownException) {
  if (!codepoints->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  return true;
}
#endif

#if defined(UCS_PLAY_DUMB)
inline
uint8_t nfd_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw(InvalidCodepointException, SizeUnknownException) {
  return UCS_QC_YES;
}
#endif

#if defined(UCS_PLAY_DUMB)
inline
DPtr<uint32_t> *nfd(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  codepoints->hold();
  return codepoints;
}
#endif

#if defined(UCS_PLAY_DUMB)
inline
DPtr<uint32_t> *nfd_opt(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  codepoints->hold();
  return codepoints;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_K)
inline
uint8_t nfkd_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw(InvalidCodepointException, SizeUnknownException) {
  return UCS_QC_YES;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_K)
inline
DPtr<uint32_t> *nfkd(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  codepoints->hold();
  return codepoints;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_K)
inline
DPtr<uint32_t> *nfkd_opt(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  codepoints->hold();
  return codepoints;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C)
inline
uint8_t nfc_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw(InvalidCodepointException, SizeUnknownException) {
  return UCS_QC_YES;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C)
inline
DPtr<uint32_t> *nfc(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  codepoints->hold();
  return codepoints;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C)
inline
DPtr<uint32_t> *nfc_opt(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  codepoints->hold();
  return codepoints;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C) && !defined(UCS_NO_K)
inline
uint8_t nfkc_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw(InvalidCodepointException, SizeUnknownException) {
  return UCS_QC_YES;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C) && !defined(UCS_NO_K)
inline
DPtr<uint32_t> *nfkc(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  codepoints->hold();
  return codepoints;
}
#endif

#if defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C) && !defined(UCS_NO_K)
inline
DPtr<uint32_t> *nfkc_opt(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  codepoints->hold();
  return codepoints;
}
#endif

}
