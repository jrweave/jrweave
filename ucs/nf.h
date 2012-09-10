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

#ifndef __UCS__NF_H__
#define __UCS__NF_H__

#include "ptr/BadAllocException.h"
#include "ptr/DPtr.h"
#include "ptr/SizeUnknownException.h"
#include "sys/ints.h"
#include "ucs/InvalidCodepointException.h"

namespace ucs {

using namespace ptr;
using namespace std;

#include "ucs_arrays.h"
// but if defined(UCS_PLAY_DUMB), do no refer to arrays

bool nfvalid(const uint32_t codepoint) throw();

bool nfvalid(DPtr<uint32_t> *codepoints) throw (SizeUnknownException);

uint8_t nfd_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw (InvalidCodepointException, SizeUnknownException);

DPtr<uint32_t> *nfd(DPtr<uint32_t> *codepoints)
    throw (InvalidCodepointException, SizeUnknownException,
    BadAllocException);

DPtr<uint32_t> *nfd_opt(DPtr<uint32_t> *codepoints)
    throw (InvalidCodepointException, SizeUnknownException,
    BadAllocException);

#if !defined(UCS_NO_K)
uint8_t nfkd_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw (InvalidCodepointException, SizeUnknownException);

DPtr<uint32_t> *nfkd(DPtr<uint32_t> *codepoints)
    throw (InvalidCodepointException, SizeUnknownException,
    BadAllocException);

DPtr<uint32_t> *nfkd_opt(DPtr<uint32_t> *codepoints)
    throw (InvalidCodepointException, SizeUnknownException,
    BadAllocException);
#endif

#if !defined(UCS_NO_C)
uint8_t nfc_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw (InvalidCodepointException, SizeUnknownException);

DPtr<uint32_t> *nfc(DPtr<uint32_t> *codepoints)
    throw (InvalidCodepointException, SizeUnknownException,
    BadAllocException);

DPtr<uint32_t> *nfc_opt(DPtr<uint32_t> *codepoints)
    throw (InvalidCodepointException, SizeUnknownException,
    BadAllocException);

#if !defined(UCS_NO_K)
uint8_t nfkc_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw (InvalidCodepointException, SizeUnknownException);

DPtr<uint32_t> *nfkc(DPtr<uint32_t> *codepoints)
    throw (InvalidCodepointException, SizeUnknownException,
    BadAllocException);

DPtr<uint32_t> *nfkc_opt(DPtr<uint32_t> *codepoints)
    throw (InvalidCodepointException, SizeUnknownException,
    BadAllocException);
#endif
#endif /* !defined(UCS_NO_C) */
}

#include "ucs/nf-inl.h"

#endif /* __UCS__NF_H__ */
