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

#include "ucs/utf.h"

#include <algorithm>
#include "sys/endian.h"

namespace ucs {

using namespace ptr;
using namespace std;
using namespace sys;

inline
size_t utf8len(const uint32_t codepoint) THROWS(InvalidEncodingException) {
  return utf8len(codepoint, NULL);
}
TRACE(InvalidEncodingException, "(trace)")

inline
uint32_t utf8char(const uint8_t *utf8str) THROWS(InvalidEncodingException) {
  return utf8char(utf8str, NULL);
}
TRACE(InvalidEncodingException, "(trace)")

template<class input_iter, class output_iter>
size_t utf8enc(input_iter begin, input_iter end, output_iter out)
    THROWS(InvalidEncodingException) {
  size_t total_len = 0;
  for (; begin != end; begin++) {
    uint8_t buf[4];
    size_t len = utf8len(*begin, buf);
    copy(buf, buf + len, out);
    out += len;
    total_len += len;
  }
  return total_len;
}
TRACE(InvalidEncodingException, "(trace)")

inline
size_t utf16len(const uint32_t codepoint, const enum BOM bom)
    THROWS(InvalidEncodingException) {
  return utf16len(codepoint, bom, NULL);
}
TRACE(InvalidEncodingException, "(trace)")

inline
bool utf16flip(const DPtr<uint16_t> *utf16str) THROWS(SizeUnknownException) {
  return utf16flip(utf16str, NULL);
}
TRACE(SizeUnknownException, "(trace)")

inline
uint32_t utf16char(const uint16_t *utf16str, const bool flip)
    THROWS(InvalidEncodingException) {
  return utf16char(utf16str, flip, NULL);
}
TRACE(InvalidEncodingException, "(trace)")

template<typename input_iter, typename output_iter>
size_t utf16enc(input_iter begin, input_iter end, const enum BOM bom,
    output_iter out) THROWS(InvalidEncodingException) {
  size_t total_len = 0;
  if (begin == end) {
    return 0;
  }
  if (bom != NONE) {
    if ((is_little_endian() && bom == BIG)
        || (is_big_endian() && bom == LITTLE)) {
      *out = UINT16_C(0xFFFE); // reverse
    } else {
      *out = UINT16_C(0xFEFF);
    }
    out++;
    total_len++;
  }
  for (; begin != end; begin++) {
    uint16_t buf[2];
    size_t len = utf16len(*begin, bom, buf);
    copy(buf, buf + len, out);
    out += len;
    total_len += len;
  }
  return total_len;
}
TRACE(InvalidEncodingException, "(trace)")

inline
size_t utf32len(const uint32_t codepoint, const enum BOM bom)
    THROWS(InvalidEncodingException) {
  return utf32len(codepoint, bom, NULL);
}
TRACE(InvalidEncodingException, "(trace)")

inline
bool utf32flip(const DPtr<uint32_t> *utf32str) THROWS(SizeUnknownException) {
  return utf32flip(utf32str, NULL);
}
TRACE(SizeUnknownException, "(trace)")

inline
uint32_t utf32char(const uint32_t *utf32str, const bool flip)
    THROWS(InvalidEncodingException) {
  return utf32char(utf32str, flip, NULL);
}
TRACE(InvalidEncodingException, "(trace)")

template<typename input_iter, typename output_iter>
size_t utf32enc(input_iter begin, input_iter end, const enum BOM bom,
    output_iter out) THROWS(InvalidEncodingException) {
  size_t total_len = 0;
  if (begin == end) {
    return 0;
  }
  if (bom != NONE) {
    if ((is_little_endian() && bom == BIG)
        || (is_big_endian() && bom == LITTLE)) {
      *out = UINT32_C(0xFFFE0000); // reverse
    } else {
      *out = UINT32_C(0x0000FEFF);
    }
    out++;
    total_len++;
  }
  for (; begin != end; begin++) {
    utf32len(*begin, bom, &(*out));
    out++;
    total_len++;
  }
  return total_len;
}
TRACE(InvalidEncodingException, "(trace)")

}
