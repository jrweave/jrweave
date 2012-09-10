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
#include <iostream>
#include <sstream>
#include "ptr/MPtr.h"
#include "sys/endian.h"
#include "ucs/nf.h"
#include "util/funcs.h"

namespace ucs {

using namespace ex;
using namespace ptr;
using namespace std;
using namespace sys;
using namespace util;

// utf8val is assumed to be long enough for encoded value
// (if not NULL); length >= 4 is always safe.  It is always
// filled in big-endian order.
size_t utf8len(const uint32_t codepoint, uint8_t *utf8val)
    throw(InvalidEncodingException) {
  if (codepoint <= UINT32_C(0x7F)) {
    if (utf8val != NULL) {
      utf8val[0] = (uint8_t) codepoint;
    }
    return 1;
  } else if (codepoint <= UINT32_C(0x7FF)) {
    if (utf8val != NULL) {
      utf8val[0] = ((uint8_t)(codepoint >> 6)) | UINT8_C(0xC0);
      utf8val[1] = (((uint8_t)(codepoint)) & UINT8_C(0x3F))
          | UINT8_C(0x80);
    }
    return 2;
  } else if (codepoint <= UINT32_C(0x10000)) {
    if (utf8val != NULL) {
      utf8val[0] = (((uint8_t)(codepoint >> 12)) & UINT8_C(0x0F))
          | UINT8_C(0xE0);
      utf8val[1] = (((uint8_t)(codepoint >> 6)) & UINT8_C(0x3F))
          | UINT8_C(0x80);
      utf8val[2] = (((uint8_t)(codepoint)) & UINT8_C(0x3F))
          | UINT8_C(0x80);
    }
    return 3;
  } else if (codepoint <= UINT32_C(0x10FFFF)) {
    if (utf8val != NULL) {
      utf8val[0] = (((uint8_t)(codepoint >> 18)) & UINT8_C(0x07))
          | UINT8_C(0xF0);
      utf8val[1] = (((uint8_t)(codepoint >> 12)) & UINT8_C(0x3F))
          | UINT8_C(0x80);
      utf8val[2] = (((uint8_t)(codepoint >> 6)) & UINT8_C(0x3F))
          | UINT8_C(0x80);
      utf8val[3] = (((uint8_t)(codepoint)) & UINT8_C(0x3F))
          | UINT8_C(0x80);
    }
    return 4;
  } else if (codepoint <= UINT32_C(0x1FFFFF)) {
    THROW(InvalidEncodingException, "Invalid codepoint.", codepoint);
  }
  THROW(InvalidEncodingException,
      "No valid UTF-8 encoding for invalid Unicode codepoint.", codepoint);
}

size_t utf8enc(const uint32_t *codepoints, const size_t len, uint8_t *out)
    THROWS(InvalidEncodingException) {
  size_t total_len = 0;
  const uint32_t *end = codepoints + len;
  for (; codepoints != end; codepoints++) {
    size_t len = utf8len(*codepoints, out);
    out += len;
    total_len += len;
  }
  return total_len;
}
TRACE(InvalidEncodingException, "(trace)")

DPtr<uint8_t> *utf8enc(DPtr<uint32_t> *codepoints)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException) {
  if (!codepoints->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (codepoints->size() <= 0) {
    DPtr<uint8_t> *d;
    NEW(d, MPtr<uint8_t>, (uint8_t*)NULL, 0);
    return d;
  }
  size_t len = 0;
  uint8_t *enc;
  if (!alloc(enc, codepoints->size() << 2)) {
    THROW(BadAllocException, (codepoints->size() << 2)*sizeof(uint8_t));
  }
  try {
    len = utf8enc(codepoints->dptr(), codepoints->size(), enc);
  } catch (InvalidEncodingException &e) {
    dalloc(enc);
    RETHROW(e, "(rethrow)");
  }
  if (!ralloc(enc, len)) {
    dalloc(enc);
    THROW(BadAllocException, len*sizeof(uint8_t));
  }
  DPtr<uint8_t> *d;
  NEW(d, MPtr<uint8_t>, enc, len);
  return d;
}

DPtr<uint32_t> *utf8dec(DPtr<uint8_t> *utf8str)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException) {
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (utf8str->size() <= 0) {
    DPtr<uint32_t> *d;
    NEW(d, MPtr<uint32_t>, (uint32_t*)NULL, 0);
    return d;
  }
  size_t len = 0;
  uint32_t *dec;
  if (!alloc(dec, utf8str->size())) {
    THROW(BadAllocException, utf8str->size() * sizeof(uint32_t));
  }
  int i;
  const uint8_t *at = utf8str->dptr();
  const uint8_t *end = at + utf8str->size();
  try {
    while (at != end) {
      dec[len++] = utf8char(at, &at);
    }
  } catch (InvalidEncodingException &e) {
    dalloc(dec);
    RETHROW(e, "(rethrow)");
  }
  if (!ralloc(dec, len)) {
    dalloc(dec);
    THROW(BadAllocException, len*sizeof(uint32_t));
  }
  DPtr<uint32_t> *d;
  NEW(d, MPtr<uint32_t>, dec, len);
  return d;
}

size_t utf8nchars(const DPtr<uint8_t> *utf8str)
    throw(SizeUnknownException, InvalidEncodingException) {
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint8_t *cur = utf8str->dptr();
  const uint8_t *end = cur + utf8str->size();
  size_t len;
  for (len = 0; cur != end; len++) {
    utf8char(cur, &cur);
  }
  return len;
}

uint32_t utf8char(const uint8_t *utf8str, const uint8_t **next)
    throw(InvalidEncodingException) {
  const uint8_t head = *utf8str;
  if (head <= UINT8_C(0x7F)) {
    if (next != NULL) {
      *next = utf8str + 1;
    }
    return (uint32_t) head;
  } else if (head <= UINT8_C(0xBF)) {
    THROW(InvalidEncodingException,
        "Unexpected non-header byte while UTF-8 decoding.",
        (uint32_t) head);
  } else if (head <= UINT8_C(0xDF)) {
    const uint8_t part = utf8str[1];
    if (part < UINT8_C(0x80)) {
      THROW(InvalidEncodingException,
          "Unexpected second byte while UTF-8 decoding.",
          (uint32_t) part);
    }
    if (next != NULL) {
      *next = utf8str + 2;
    }
    return ((((uint32_t) head) & UINT32_C(0x1F)) << 6)
        | (((uint32_t) part) & UINT32_C(0x3F));
  } else if (head <= UINT8_C(0xEF)) {
    const uint8_t part1 = utf8str[1];
    const uint8_t part2 = utf8str[2];
    if (part1 < UINT8_C(0x80)) {
      THROW(InvalidEncodingException,
          "Unexpected second byte while UTF-8 decoding.",
          (uint32_t) part1);
    }
    if (part2 < UINT8_C(0x80)) {
      THROW(InvalidEncodingException,
          "Unexpected third byte while UTF-8 decoding.",
          (uint32_t) part2);
    }
    if (next != NULL) {
      *next = utf8str + 3;
    }
    return ((((uint32_t) head) & UINT32_C(0x0F)) << 12)
        | ((((uint32_t) part1) & UINT32_C(0x3F)) << 6)
        | (((uint32_t) part2) & UINT32_C(0x3F));
  } else if (head <= UINT8_C(0xF7)) {
    const uint8_t part1 = utf8str[1];
    const uint8_t part2 = utf8str[2];
    const uint8_t part3 = utf8str[3];
    if (part1 < UINT8_C(0x80)) {
      THROW(InvalidEncodingException,
          "Unexpected second byte while UTF-8 decoding.",
          (uint32_t) part1);
    }
    if (part2 < UINT8_C(0x80)) {
      THROW(InvalidEncodingException,
          "Unexpected third byte while UTF-8 decoding.",
          (uint32_t) part2);
    }
    if (part3 < UINT8_C(0x80)) {
      THROW(InvalidEncodingException,
          "Unexpected fourth byte while UTF-8 decoding.",
          (uint32_t) part3);
    }
    if (next != NULL) {
      *next = utf8str + 4;
    }
    return ((((uint32_t) head) & UINT32_C(0x07)) << 18)
        | ((((uint32_t) part1) & UINT32_C(0x3F)) << 12)
        | ((((uint32_t) part2) & UINT32_C(0x3F)) << 6)
        | (((uint32_t) part3) & UINT32_C(0x3F));
  }
  THROW(InvalidEncodingException,
      "Unexpected non-header byte while UTF-8 decoding.",
      (uint32_t) head);
}

void utf8validate(DPtr<uint8_t> *utf8str)
    throw(InvalidCodepointException, InvalidEncodingException,
          SizeUnknownException) {
  if (utf8str == NULL || !utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  uint8_t *begin = utf8str->dptr();
  uint8_t *end = begin + utf8str->size();
  while (begin != end) {
    uint32_t codepoint;
    try {
      codepoint = utf8char(begin, (const uint8_t **)&begin);
    } JUST_RETHROW(InvalidEncodingException, "(rethrow)")
    if (!nfvalid(codepoint)) {
      THROW(InvalidCodepointException, codepoint);
    }
  }
}

size_t utf16len(const uint32_t codepoint, const enum BOM bom,
    uint16_t *utf16val) throw(InvalidEncodingException) {
  if (UINT32_C(0xD800) <= codepoint && codepoint <= UINT32_C(0xDFFF)) {
    THROW(InvalidEncodingException,
        "Surrogate pair character cannot be encoded in UTF-16.", codepoint);
  }
  size_t len;
  if (codepoint <= UINT32_C(0xFFFF)) {
    if (utf16val != NULL) {
      *utf16val = codepoint;
    }
    len = 1;
  } else if (codepoint <= UINT32_C(0x10FFFF)) {
    if (utf16val != NULL) {
      const uint32_t adjusted = codepoint - UINT32_C(0x10000);
      utf16val[0] = UINT16_C(0xD800)
          + ((uint16_t)(adjusted >> 10));
      utf16val[1] = UINT16_C(0xDC00)
          + (((uint16_t) adjusted) & UINT16_C(0x3FF));
    }
    len = 2;
  } else if (codepoint <= UINT32_C(0x1FFFFF)) {
    THROW(InvalidEncodingException, "Invalid codepoint.", codepoint);
  } else {
    THROW(InvalidEncodingException,
        "No valid UTF-16 encoding for invalid codepoint.", codepoint);
  }
  if (utf16val != NULL && bom != ANY) {
    if ((is_little_endian() && (bom == BIG || bom == NONE))
        || (is_big_endian() && bom == LITTLE)) {
      size_t i;
      for (i = 0; i < len; i++) {
        reverse_bytes<uint16_t>(utf16val[i]);
      }
    }
  }
  return len;
}

size_t utf16enc(const uint32_t *codepoints, const size_t len,
    const enum BOM bom, uint16_t *out)
    THROWS(InvalidEncodingException) {
  size_t total_len = 0;
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
  const uint32_t *end = codepoints + len;
  for (; codepoints != end; codepoints++) {
    size_t len = utf16len(*codepoints, bom, out);
    out += len;
    total_len += len;
  }
  return total_len;
}
TRACE(InvalidEncodingException, "(trace)")

DPtr<uint16_t> *utf16enc(DPtr<uint32_t> *codepoints, const enum BOM bom)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException) {
  if (!codepoints->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (codepoints->size() <= 0) {
    DPtr<uint16_t> *d;
    NEW(d, MPtr<uint16_t>, (uint16_t*)NULL, 0);
    return d;
  }
  size_t len = 0;
  size_t newsize = (codepoints->size() << 1) + (bom == NONE ? 0 : 1);
  uint16_t *enc;
  if (!alloc(enc, newsize)) {
    THROW(BadAllocException, newsize*sizeof(uint16_t));
  }
  try {
    len = utf16enc(codepoints->dptr(), codepoints->size(), bom, enc);
  } catch (InvalidEncodingException &e) {
    dalloc(enc);
    RETHROW(e, "(rethrow)");
  }
  if (!ralloc(enc, len)) {
    dalloc(enc);
    THROW(BadAllocException, len*sizeof(uint16_t));
  }
  DPtr<uint16_t> *d;
  NEW(d, MPtr<uint16_t>, enc, len);
  return d;
}

DPtr<uint32_t> *utf16dec(DPtr<uint16_t> *utf16str)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException) {
  if (!utf16str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (utf16str->size() <= 0) {
    DPtr<uint32_t> *d;
    NEW(d, MPtr<uint32_t>, (uint32_t*)NULL, 0);
    return d;
  }
  const uint16_t *at;
  bool flip = utf16flip(utf16str, &at);
  size_t newsize = utf16str->size() - (at - utf16str->dptr());
  uint32_t *dec;
  if (!alloc(dec, newsize)) {
    THROW(BadAllocException, newsize * sizeof(uint32_t));
  }
  const uint16_t *end = utf16str->dptr() + utf16str->size();
  size_t len;
  for (len = 0; at != end; len++) {
    dec[len] = utf16char(at, flip, &at);
  }
  if (!ralloc(dec, len)) {
    dalloc(dec);
    THROW(BadAllocException, len * sizeof(uint32_t));
  }
  DPtr<uint32_t> *d;
  NEW(d, MPtr<uint32_t>, dec, len);
  return d;
}

size_t utf16nchars(const DPtr<uint16_t> *utf16str) throw(SizeUnknownException,
    InvalidEncodingException) {
  if (!utf16str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint16_t *cur;
  utf16flip(utf16str, &cur);
  const uint16_t *end = cur + utf16str->size();
  size_t len;
  for (len = 0; cur != end; len++) {
    utf16char(cur, false, &cur);
  }
  return len;
}

bool utf16flip(const DPtr<uint16_t> *utf16str, const uint16_t **start)
    throw(SizeUnknownException) {
  if (!utf16str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (utf16str->size() == 0) {
    if (start != NULL) {
      *start = utf16str->dptr();
    }
    return is_little_endian();
  }
  const uint16_t first = *(utf16str->dptr());
  if (first == UINT16_C(0xFEFF)) {
    if (start != NULL) {
      *start = utf16str->dptr() + 1;
    }
    return false;
  }
  if (first == UINT16_C(0xFFFE)) {
    if (start != NULL) {
      *start = utf16str->dptr() + 1;
    }
    return true;
  }
  if (start != NULL) {
    *start = utf16str->dptr();
  }
  return is_little_endian();
}

uint32_t utf16char(const uint16_t *utf16str, const bool flip,
    const uint16_t **next) throw(InvalidEncodingException) {
  uint16_t head = *utf16str;
  if (flip) {
    reverse_bytes<uint16_t>(head);
  }
  if (UINT16_C(0xD800) <= head && head <= UINT16_C(0xDBFF)) {
    uint16_t tail = utf16str[1];
    if (flip) {
      reverse_bytes<uint16_t>(tail);
    }
    if (tail < UINT16_C(0xDC00) || UINT16_C(0xDFFF) < tail) {
      THROW(InvalidEncodingException, "Expected lower surrogate.",
          (uint32_t) tail);
    }
    if (next != NULL) {
      *next = utf16str + 2;
    }
    return (((((uint32_t)head) - UINT32_C(0xD800)) << 10)
        | (((uint32_t)tail) - UINT32_C(0xDC00))) + UINT32_C(0x10000);
  }
  if (next != NULL) {
    *next = utf16str + 1;
  }
  return (uint32_t) head;
}

void utf16validate(DPtr<uint16_t> *utf16str)
    throw(InvalidCodepointException, InvalidEncodingException,
          SizeUnknownException) {
  uint16_t *begin;
  bool flip;
  try {
    flip = utf16flip(utf16str, (const uint16_t **)&begin);
  } JUST_RETHROW(SizeUnknownException, "(rethrow)")
  uint16_t *end = utf16str->dptr() + utf16str->size();
  while (begin != end) {
    uint32_t codepoint;
    try {
      codepoint = utf16char(begin, flip, (const uint16_t **)&begin);
    } JUST_RETHROW(InvalidEncodingException, "(rethrow)")
    if (!nfvalid(codepoint)) {
      THROW(InvalidCodepointException, codepoint);
    }
  }
}

size_t utf32len(const uint32_t codepoint, const enum BOM bom,
    uint32_t *utf32val) throw(InvalidEncodingException) {
  if (codepoint > UINT32_C(0x10FFFF)) {
    THROW(InvalidEncodingException, "Invalid codepoint.", codepoint);
  }
  if (utf32val != NULL) {
    *utf32val = codepoint;
    if (bom != ANY) {
      if ((is_little_endian() && (bom == BIG || bom == NONE))
          || (is_big_endian() && bom == LITTLE)) {
        reverse_bytes<uint32_t>(*utf32val);
      }
    }
  }
  return 1;
}

size_t utf32enc(const uint32_t *codepoints, const size_t len,
    const enum BOM bom, uint32_t *out)
    THROWS(InvalidEncodingException) {
  size_t total_len = 0;
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
  if (bom == NONE && is_big_endian()) {
    memmove(out, codepoints, len*sizeof(uint32_t));
    total_len += len;
  } else {
    const uint32_t *end = codepoints + len;
    for (; codepoints != end; codepoints++) {
      utf32len(*codepoints, bom, out);
      out++;
      total_len++;
    }
  }
  return total_len;
}
TRACE(InvalidEncodingException, "(trace)")

DPtr<uint32_t> *utf32enc(DPtr<uint32_t> *codepoints,
    const enum BOM bom)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException) {
  if (!codepoints->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (codepoints->size() <= 0) {
    codepoints->hold();
    return codepoints;
  }
  if (bom == NONE && is_big_endian()) {
    const uint32_t *begin = codepoints->dptr();
    const uint32_t *end = begin + codepoints->size();
    for (; begin != end; ++begin) {
      // Just for simple validation.
      utf32len(*begin, bom, NULL);
    }
    codepoints->hold();
    return codepoints;
  }
  size_t len = 0;
  uint32_t *enc;
  if (!alloc(enc, (codepoints->size() << 1) + (bom == NONE ? 0 : 1))) {
    THROW(BadAllocException, (codepoints->size() << 1)*sizeof(uint32_t));
  }
  try {
    len = utf32enc(codepoints->dptr(), codepoints->size(), bom, enc);
  } catch (InvalidEncodingException &e) {
    dalloc(enc);
    RETHROW(e, "(rethrow)");
  }
  DPtr<uint32_t> *d;
  NEW(d, MPtr<uint32_t>, enc, len);
  return d;
}

DPtr<uint32_t> *utf32dec(DPtr<uint32_t> *utf32str)
    throw(SizeUnknownException, InvalidEncodingException, BadAllocException) {
  if (!utf32str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (utf32str->size() <= 0) {
    utf32str->hold();
    return utf32str;
  }
  const uint32_t *at;
  bool flip = utf32flip(utf32str, &at);
  if (!flip) {
    if (at == utf32str->dptr()) { 
      utf32str->hold();
      return utf32str;
    } else {
      return utf32str->sub(at - utf32str->dptr());
    }
  }
  size_t len = 0;
  size_t newsize = utf32str->size() - (at - utf32str->dptr());
  uint32_t *dec;
  if (!alloc(dec, newsize)) {
    THROW(BadAllocException, newsize * sizeof(uint32_t));
  }

  //////////
  // As an optimization, use memcpy instead of utf32char.
  memcpy(dec, at, newsize * sizeof(uint32_t));
  if (flip) {
    size_t i;
    for (i = 0; i < newsize; i++) {
      reverse_bytes<uint32_t>(dec[i]);
    }
  }
  DPtr<uint32_t> *d;
  NEW(d, MPtr<uint32_t>, dec, newsize);
  return d;
}

size_t utf32nchars(const DPtr<uint32_t> *utf32str) throw(SizeUnknownException,
    InvalidEncodingException) {
  if (!utf32str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint32_t *cur;
  utf32flip(utf32str, &cur);
  const uint32_t *end = cur + utf32str->size();
  size_t len;
  for (len = 0; cur != end; len++) {
    utf32char(cur, false, &cur);
  }
  return len;
}

bool utf32flip(const DPtr<uint32_t> *utf32str, const uint32_t **start)
    throw(SizeUnknownException) {
  if (!utf32str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (utf32str->size() == 0) {
    if (start != NULL) {
      *start = utf32str->dptr();
    }
    return is_little_endian();
  }
  const uint32_t first = *(utf32str->dptr());
  if (first == UINT32_C(0x0000FEFF)) {
    if (start != NULL) {
      *start = utf32str->dptr() + 1;
    }
    return false;
  }
  if (first == UINT32_C(0xFFFE0000)) {
    if (start != NULL) {
      *start = utf32str->dptr() + 1;
    }
    return true;
  }
  if (start != NULL) {
    *start = utf32str->dptr();
  }
  return is_little_endian();
}

uint32_t utf32char(const uint32_t *utf32str, const bool flip,
    const uint32_t **next) THROWS(InvalidEncodingException) {
  uint32_t c = *utf32str;
  if (flip) {
    reverse_bytes<uint32_t>(c);
  }
  if (next != NULL) {
    *next = utf32str + 1;
  }
  return c;
}
TRACE(InvalidEncodingException, "(trace)")

void utf32validate(DPtr<uint32_t> *utf32str)
    throw(InvalidCodepointException, InvalidEncodingException,
          SizeUnknownException) {
  uint32_t *begin;
  bool flip;
  try {
    flip = utf32flip(utf32str, (const uint32_t **)&begin);
  } JUST_RETHROW(SizeUnknownException, "(rethrow)")
  uint32_t *end = utf32str->dptr() + utf32str->size();
  while (begin != end) {
    uint32_t codepoint;
    try {
      codepoint = utf32char(begin, flip, (const uint32_t **)&begin);
    } JUST_RETHROW(InvalidEncodingException, "(rethrow)")
    if (!nfvalid(codepoint)) {
      THROW(InvalidCodepointException, codepoint);
    }
  }
}

}
