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

#ifndef __UCS__UTF_H__
#define __UCS__UTF_H__

#include "ex/TraceableException.h"
#include "ptr/BadAllocException.h"
#include "ptr/DPtr.h"
#include "ptr/SizeUnknownException.h"
#include "sys/ints.h"
#include "ucs/InvalidCodepointException.h"
#include "ucs/InvalidEncodingException.h"

namespace ucs {

using namespace ptr;
using namespace std;

enum BOM {
  LITTLE, ANY, NONE, BIG
};

// Returns number of uint8_t
size_t utf8len(const uint32_t codepoint) throw(InvalidEncodingException);

size_t utf8len(const uint32_t codepoint, uint8_t *utf8val)
    throw(InvalidEncodingException);

template<typename input_iter, typename output_iter>
size_t utf8enc(input_iter begin, input_iter end, output_iter out)
    throw(InvalidEncodingException);

size_t utf8enc(const uint32_t *codepoints, const size_t len, uint8_t *out)
    throw(InvalidEncodingException);

DPtr<uint8_t> *utf8enc(DPtr<uint32_t> *codepoints)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException);

DPtr<uint32_t> *utf8dec(DPtr<uint8_t> *utf8str)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException);

size_t utf8nchars(const DPtr<uint8_t> *utf8str) throw(SizeUnknownException,
    InvalidEncodingException);

uint32_t utf8char(const uint8_t *utf8str) throw(InvalidEncodingException);

uint32_t utf8char(const uint8_t *utf8str, const uint8_t **next)
    throw(InvalidEncodingException);

void utf8validate(DPtr<uint8_t> *utf8str)
    throw(InvalidCodepointException, InvalidEncodingException,
          SizeUnknownException);

// Returns number of uint16_t
size_t utf16len(const uint32_t codepoint, const enum BOM bom)
    throw(InvalidEncodingException);

size_t utf16len(const uint32_t codepoint, const enum BOM bom,
    uint16_t *utf16val) throw(InvalidEncodingException);

template<typename input_iter, typename output_iter>
size_t utf16enc(input_iter begin, input_iter end, const enum BOM bom,
    output_iter out) throw(InvalidEncodingException);

size_t utf16enc(const uint32_t *codepoints, const size_t len,
    const enum BOM bom, uint16_t *out) throw(InvalidEncodingException);

DPtr<uint16_t> *utf16enc(DPtr<uint32_t> *codepoints, const enum BOM bom)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException);

DPtr<uint32_t> *utf16dec(DPtr<uint16_t> *utf16str)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException);

size_t utf16nchars(const DPtr<uint16_t> *utf16str) throw(SizeUnknownException,
    InvalidEncodingException);

bool utf16flip(const DPtr<uint16_t> *utf16str) throw(SizeUnknownException);

bool utf16flip(const DPtr<uint16_t> *utf16str, const uint16_t **start)
    throw(SizeUnknownException);

uint32_t utf16char(const uint16_t *utf16str, const bool flip)
    throw(InvalidEncodingException);

uint32_t utf16char(const uint16_t *utf16str, const bool flip,
    const uint16_t **next) throw(InvalidEncodingException);

void utf16validate(DPtr<uint16_t> *utf16str)
    throw(InvalidCodepointException, InvalidEncodingException,
          SizeUnknownException);

// Returns number of uint32_t
size_t utf32len(const uint32_t codepoint, const enum BOM bom)
    throw(InvalidEncodingException);

size_t utf32len(const uint32_t codepoint, const enum BOM bom,
    uint32_t *utf32val) throw(InvalidEncodingException);

template<typename input_iter, typename output_iter>
size_t utf32enc(input_iter begin, input_iter end, const enum BOM bom,
    output_iter out) throw(InvalidEncodingException);

size_t utf32enc(const uint32_t *codepoints, const size_t len,
    const enum BOM bom, uint32_t *out) throw(InvalidEncodingException);

DPtr<uint32_t> *utf32enc(DPtr<uint32_t> *codepoints, const enum BOM bom)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException);

DPtr<uint32_t> *utf32dec(DPtr<uint32_t> *utf32str)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException);

size_t utf32nchars(const DPtr<uint32_t> *utf32str) throw(SizeUnknownException,
    InvalidEncodingException);

bool utf32flip(const DPtr<uint32_t> *utf32str) throw(SizeUnknownException);

bool utf32flip(const DPtr<uint32_t> *utf32str, const uint32_t **start)
    throw(SizeUnknownException);

uint32_t utf32char(const uint32_t *utf32str, const bool flip)
    throw(InvalidEncodingException);

uint32_t utf32char(const uint32_t *utf32str, const bool flip,
    const uint32_t **next) throw(InvalidEncodingException);

void utf32validate(DPtr<uint32_t> *utf32str)
    throw(InvalidCodepointException, InvalidEncodingException,
          SizeUnknownException);

}

#include "ucs/utf-inl.h"

#endif /* __UCS__UTF_H__ */
