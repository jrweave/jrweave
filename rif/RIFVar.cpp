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

#include "rif/RIFVar.h"

#include <algorithm>
#include "rif/RIFConst.h"
#include "ucs/UTF8Iter.h"
#include "ucs/nf.h"
#include "ucs/utf.h"

namespace rif {

using namespace ptr;
using namespace std;
using namespace ucs;

RIFVar::RIFVar(DPtr<uint8_t> *utf8name)
    throw(BaseException<void*>, SizeUnknownException,
          InvalidCodepointException, InvalidEncodingException)
    : normalized(false) {
  if (utf8name == NULL) {
    THROW(BaseException<void*>, NULL, "utf8name cannot be NULL.");
  }
  if (!utf8name->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  try {
    utf8validate(utf8name);
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(InvalidEncodingException, "(rethrow)")
  this->utf8name = utf8name;
  this->utf8name->hold();
}

int RIFVar::cmp(const RIFVar &var1, const RIFVar &var2) throw() {
  if (&var1 == &var2) {
    return 0;
  }
  DPtr<uint8_t> *name1 = var1.getName();
  DPtr<uint8_t> *name2 = var2.getName();
  size_t size1 = name1->size();
  size_t size2 = name2->size();
  size_t len = min(size1, size2);
  int cmp = memcmp(name1->dptr(), name2->dptr(), len * sizeof(uint8_t));
  name1->drop();
  name2->drop();
  if (cmp != 0) {
    return cmp;
  }
  return size1 < size2 ? -1 :
        (size1 > size2 ?  1 : 0);
}

RIFVar RIFVar::parse(DPtr<uint8_t> *utf8str)
    throw(BadAllocException, InvalidCodepointException,
          InvalidEncodingException, BaseException<void*>,
          SizeUnknownException, TraceableException) {
  if (utf8str == NULL) {
    THROW(BaseException<void*>, NULL, "utf8str must not be NULL.");
  }
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  size_t sz = utf8str->size();
  const uint8_t *p = utf8str->dptr();
  if (utf8str->size() <= 0 || *p != to_ascii('?')) {
    THROW(TraceableException,
          "Parsing utf8str into RIFVar; must start with '?'.");
  }
  DPtr<uint8_t> *name;
  if (sz < 3 || p[1] != to_ascii('"') || p[sz - 1] != to_ascii('"')) {
    name = utf8str->sub(1, sz - 1);
    UTF8Iter begin(name);
    UTF8Iter end(name);
    end.finish();
    if (!isNCName(begin, end)) {
      name->drop();
      THROW(TraceableException,
            "Var name must be NCName or Unicode string in quotes.");
    }
  } else {
    DPtr<uint8_t> *unistr = utf8str->sub(2, sz - 3);
    try {
      name = RIFConst::unescape(unistr);
    } catch (InvalidCodepointException &e) {
      unistr->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidEncodingException &e) {
      unistr->drop();
      RETHROW(e, "(rethrow)");
    } catch (TraceableException &e) {
      unistr->drop();
      RETHROW(e, "(rethrow)");
    }
    unistr->drop();
  }
  try {
    RIFVar var(name);
    name->drop();
    return var;
  } catch (InvalidCodepointException &e) {
    name->drop();
    RETHROW(e, "(rethrow)");
  } catch (InvalidEncodingException &e) {
    name->drop();
    RETHROW(e, "(rethrow)");
  }
}

RIFVar &RIFVar::normalize() throw(BadAllocException) {
  if (this->normalized) {
    return *this;
  }
  const uint8_t *begin = this->utf8name->dptr();
  const uint8_t *end = begin + this->utf8name->size();
  for (; begin != end && is_ascii(*begin); ++begin) {
    // see if all ASCII
  }
  if (begin != end) {
    DPtr<uint32_t> *codepoints;
    try {
      codepoints = utf8dec(this->utf8name);
    } JUST_RETHROW(BadAllocException, "(rethrow)")
    DPtr<uint32_t> *nfcpoints;
    try {
      nfcpoints = nfc_opt(codepoints);
    } catch (BadAllocException &e) {
      codepoints->drop();
      RETHROW(e, "(rethrow)");
    }
    if (nfcpoints != codepoints) {
      DPtr<uint8_t> *newname;
      try {
        newname = utf8enc(nfcpoints);
      } catch (BadAllocException &e) {
        codepoints->drop();
        nfcpoints->drop();
        RETHROW(e, "(rethrow)");
      }
      this->utf8name->drop();
      this->utf8name = newname;
    }
    codepoints->drop();
    nfcpoints->drop();
  }
  if (this->utf8name->standable()) {
    this->utf8name = this->utf8name->stand();
  }
  this->normalized = true;
  return *this;
}

DPtr<uint8_t> *RIFVar::getName() const throw() {
  if (this->utf8name == NULL) {
    MPtr<uint8_t> *empty;
    NEW(empty, MPtr<uint8_t>);
    return empty;
  }
  this->utf8name->hold();
  return this->utf8name;
}

DPtr<uint8_t> *RIFVar::toUTF8String() const throw(BadAllocException) {
  DPtr<uint8_t> *utf8str;
  if (this->utf8name == NULL) {
    try {
      NEW(utf8str, MPtr<uint8_t>, 3);
    } RETHROW_BAD_ALLOC
    ascii_strcpy(utf8str->dptr(), "?\"\"");
    return utf8str;
  }
  UTF8Iter begin(this->utf8name);
  UTF8Iter end(this->utf8name);
  end.finish();
  if (isNCName(begin, end)) {
    try {
      NEW(utf8str, MPtr<uint8_t>, this->utf8name->size() + 1);
    } RETHROW_BAD_ALLOC
    **utf8str = to_ascii('?');
    memcpy(utf8str->dptr() + 1, this->utf8name->dptr(),
           this->utf8name->size() * sizeof(uint8_t));
    return utf8str;
  }
  DPtr<uint8_t> *esc;
  try {
    esc = RIFConst::escape(this->utf8name);
  } JUST_RETHROW(BadAllocException, "(rethrow)")
  try {
    NEW(utf8str, MPtr<uint8_t>, esc->size() + 3);
  } catch (BadAllocException &e) {
    esc->drop();
    RETHROW(e, "(rethrow)");
  } catch (bad_alloc &e) {
    esc->drop();
    THROW(BadAllocException, sizeof(MPtr<uint8_t>));
  }
  uint8_t *p = utf8str->dptr();
  ascii_strcpy(p, "?\"");
  p += 2;
  memcpy(p, esc->dptr(), esc->size() * sizeof(uint8_t));
  p += esc->size();
  *p = to_ascii('"');
  esc->drop();
  return utf8str;
}

RIFVar &RIFVar::operator=(const RIFVar &rhs) throw() {
  if (this != &rhs) {
    if (this->utf8name != NULL) {
      this->utf8name->drop();
    }
    this->utf8name = rhs.utf8name;
    if (this->utf8name != NULL) {
      this->utf8name->hold();
    }
    this->normalized = rhs.normalized;
  }
  return *this;
}

}
