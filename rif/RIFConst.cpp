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

#include "rif/RIFConst.h"

#include <algorithm>
#include "rdf/RDFTerm.h"
#include "sys/char.h"
#include "ucs/nf.h"
#include "ucs/utf.h"

namespace rif {

using namespace iri;
using namespace ptr;
using namespace rdf;
using namespace std;
using namespace ucs;

RIFConst::RIFConst(DPtr<uint8_t> *utf8str, const IRIRef &dt)
    throw(SizeUnknownException, BaseException<void*>, BaseException<IRIRef>,
          InvalidCodepointException, InvalidEncodingException)
    : datatype(dt), normalized(false) {
  if (utf8str == NULL) {
    THROW(BaseException<void*>, NULL, "utf8str must not be NULL.");
  }
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (dt.isRelativeRef()) {
    THROW(BaseException<IRIRef>, dt, "dt must not be a relative IRI.");
  }
  try {
    utf8validate(utf8str);
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(InvalidEncodingException, "(rethrow)")
  this->lex = utf8str;
  this->lex->hold();
}

int RIFConst::cmp(const RIFConst &rc1, const RIFConst &rc2) throw() {
  if (&rc1 == &rc2) {
    return 0;
  }
  int cmp = IRIRef::cmp(rc1.datatype, rc2.datatype);
  if (cmp != 0) {
    return cmp;
  }
  size_t len = min(rc1.lex->size(), rc2.lex->size());
  cmp = memcmp(rc1.lex->dptr(), rc2.lex->dptr(), len * sizeof(uint8_t));
  if (cmp != 0) {
    return cmp;
  }
  return rc1.lex->size() < rc2.lex->size() ? -1 :
         rc1.lex->size() > rc2.lex->size() ?  1 : 0;
}

DPtr<uint8_t> *RIFConst::escape(DPtr<uint8_t> *lex)
    throw(SizeUnknownException, BadAllocException, BaseException<void*>) {
  if (lex == NULL) {
    THROW(BaseException<void*>, NULL, "lex must not be NULL.");
  }
  if (!lex->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  DPtr<uint8_t> *almost;
  try {
    almost = RDFTerm::escape(lex, false);
  } RETHROW_BAD_ALLOC
  uint8_t *begin = almost->dptr();
  uint8_t *end = begin + almost->size();
  uint8_t *mark = begin;
  for (; mark != end && *mark != to_ascii('\b') && *mark != to_ascii('\f');
         ++mark) {
    // find first character needing escape, or end
  }
  if (mark == end) {
    return almost;
  }
  vector<uint8_t> vec;
  vec.reserve(almost->size() + 1);
  do {
    vec.insert(vec.end(), begin, mark);
    if (*mark == to_ascii('\b')) {
      vec.push_back(to_ascii('\\'));
      vec.push_back(to_ascii('b'));
    } else if (*mark == to_ascii('\f')) {
      vec.push_back(to_ascii('\\'));
      vec.push_back(to_ascii('f'));
    } else {
      // should never happen
      vec.push_back(*mark);
    }
    begin = mark + 1;
    for (mark = begin;
         mark != end && *mark != to_ascii('\b') && *mark != to_ascii('\f');
         ++mark) {
      // find next character needing escape, or end
    }
  } while(mark != end);
  vec.insert(vec.end(), begin, mark);
  almost->drop();
  DPtr<uint8_t> *p;
  try {
    NEW(p, MPtr<uint8_t>, vec.size());
  } RETHROW_BAD_ALLOC
  copy(vec.begin(), vec.end(), p->dptr());
  return p;
}

DPtr<uint8_t> *RIFConst::unescape(DPtr<uint8_t> *lex)
    throw(SizeUnknownException, BadAllocException, BaseException<void*>,
          TraceableException) {
  if (lex == NULL) {
    THROW(BaseException<void*>, NULL, "lex must not be NULL.");
  }
  if (!lex->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  bool escape_found = false;
  const uint8_t *mark = lex->dptr();
  const uint8_t *end = mark + lex->size();
  for (; mark != end && *mark != to_ascii('\\'); ++mark) {
    // find first escape, or end
  }
  if (mark == end) {
    lex->hold();
    return lex;
  }
  DPtr<uint8_t> *tmp;
  try {
    NEW(tmp, MPtr<uint8_t>, lex->size());
  } RETHROW_BAD_ALLOC
  const uint8_t *begin = lex->dptr();
  uint8_t *temp = tmp->dptr();
  do {
    for (++mark; mark != end; ++mark) {
      if (*(mark - 1) == to_ascii('\\')) {
        bool found = true;
        const uint8_t *back = mark - 1;
        if (back != begin) {
          for (--back; back != begin && *back == to_ascii('\\'); --back) {
            found = !found;
          }
          if (back == begin && *back == to_ascii('\\')) {
            found = !found;
          }
        }
        if (found && (*mark == to_ascii('b') || *mark == to_ascii('f'))) {
          break;
        }
      }
    }
    size_t sz = mark - begin - (mark == end ? 0 : 1);
    memcpy(temp, begin, sz * sizeof(uint8_t));
    temp += sz;
    if (mark != end) {
      *temp = (*mark == to_ascii('b') ? to_ascii('\b') : to_ascii('\f'));
      ++temp;
    }
    begin = mark + 1;
  } while (mark != end);
  DPtr<uint8_t> *myesc = tmp->sub(0, temp - tmp->dptr());
  tmp->drop();
  DPtr<uint8_t> *unesc;
  try {
    unesc = RDFTerm::unescape(myesc, false);
  } catch (BadAllocException &e) {
    myesc->drop();
    RETHROW(e, "(rethrow)");
  } catch (TraceableException &e) {
    myesc->drop();
    RETHROW(e, "(rethrow)");
  }
  myesc->drop();
  return unesc;
}

RIFConst RIFConst::parse(DPtr<uint8_t> *utf8str) throw(BadAllocException,
    InvalidCodepointException, InvalidEncodingException, TraceableException,
    MalformedIRIRefException, SizeUnknownException) {
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  if (end - begin < 6) {
    THROW(TraceableException, "Malformed RIFConst.");
  }
  if (*begin != to_ascii('"') || *(end - 1) != to_ascii('>')) {
    THROW(TraceableException,
          "Malformed RIFConst.  Should start with '\"' and end with '>'.");
  }
  const uint8_t *mark = begin + 1;
  bool found = false;
  for (; mark != end; ++mark) {
    if (*mark == to_ascii('"')) {
      const uint8_t *back = mark - 1;
      found = true;
      for (; back != begin && *back == to_ascii('\\'); --back) {
        found = !found;
      }
      if (back == begin && *back == to_ascii('\\')) {
        found = !found;
      }
      if (found) {
        break;
      }
    }
  }
  if (!found) {
    THROW(TraceableException,
          "Malformed RIFConst.  Could not find closing quote.");
  }
  if (ascii_strncmp(mark + 1, "^^<", 3) != 0) {
    THROW(TraceableException,
          "Malformed RIFConst.  Couldn't find \"^^<\" between lex and type.");
  }
  DPtr<uint8_t> *esclex = utf8str->sub(1, mark - begin - 1);
  DPtr<uint8_t> *lex;
  try {
    lex = RIFConst::unescape(esclex);
  } catch (BadAllocException &e) {
    esclex->drop();
    RETHROW(e, "(rethrow)");
  } catch (TraceableException &e) {
    esclex->drop();
    RETHROW(e, "(rethrow)");
  }
  esclex->drop();
  DPtr<uint8_t> *iristr = utf8str->sub(mark - begin + 4, end - mark - 5);
  try {
    IRIRef iriref(iristr);
    iristr->drop();
    RIFConst rifconst(lex, iriref);
    lex->drop();
    return rifconst;
  } catch (MalformedIRIRefException &e) {
    lex->drop();
    iristr->drop();
    RETHROW(e, "(rethrow)");
  } catch (InvalidCodepointException &e) {
    lex->drop();
    iristr->drop();
    RETHROW(e, "(rethrow)");
  } catch (InvalidEncodingException &e) {
    lex->drop();
    iristr->drop();
    RETHROW(e, "(rethrow)");
  }
}

RIFConst &RIFConst::normalize() throw(BadAllocException) {
  if (this->normalized) {
    return *this;
  }
  try {
    this->datatype.normalize();
  } JUST_RETHROW(BadAllocException, "(rethrow)")
  const uint8_t *begin = this->lex->dptr();
  const uint8_t *end = begin + this->lex->size();
  for (; begin != end && is_ascii(*begin); ++begin) {
    // see if all ASCII
  }
  // if all ASCII, don't bother with NFC normalization
  if (begin != end) {
    DPtr<uint32_t> *codepoints;
    try {
      codepoints = utf8dec(this->lex);
    } JUST_RETHROW(BadAllocException, "(rethrow)")
    DPtr<uint32_t> *nfcnorm;
    try {
      nfcnorm = nfc_opt(codepoints);
    } catch (BadAllocException &e) {
      codepoints->drop();
      RETHROW(e, "(rethrow)");
    }
    codepoints->drop();
    DPtr<uint8_t> *nfcbytes;
    try {
      nfcbytes = utf8enc(nfcnorm);
    } catch (BadAllocException &e) {
      nfcnorm->drop();
      RETHROW(e, "(rethrow)");
    }
    nfcnorm->drop();
    this->lex->drop();
    this->lex = nfcbytes;
  }
  if (this->lex->standable()) {
    this->lex = this->lex->stand();
  }
  this->normalized = true;
  return *this;
}

DPtr<uint8_t> *RIFConst::toUTF8String() const throw(BadAllocException) {
  DPtr<uint8_t> *esclex;
  try {
    esclex = RIFConst::escape(this->lex);
  } JUST_RETHROW(BadAllocException, "(rethrow)")
  DPtr<uint8_t> *iristr = this->datatype.getUTF8String();
  size_t sz = esclex->size() + iristr->size() + 6;
  DPtr<uint8_t> *utf8str;
  try {
    NEW(utf8str, MPtr<uint8_t>, sz);
  } catch (BadAllocException &e) {
    esclex->drop();
    iristr->drop();
    RETHROW(e, "(rethrow)");
  } catch (bad_alloc &e) {
    esclex->drop();
    iristr->drop();
    THROW(BadAllocException, sizeof(MPtr<uint8_t>));
  }
  uint8_t *utf8p = utf8str->dptr();
  *utf8p = to_ascii('"');
  memcpy(++utf8p, esclex->dptr(), esclex->size() * sizeof(uint8_t));
  utf8p += esclex->size();
  ascii_strcpy(utf8p, "\"^^<");
  utf8p += 4;
  memcpy(utf8p, iristr->dptr(), iristr->size() * sizeof(uint8_t));
  utf8p += iristr->size();
  *utf8p = to_ascii('>');
  esclex->drop();
  iristr->drop();
  return utf8str;
}

RIFConst &RIFConst::operator=(const RIFConst &rhs) throw() {
  if (this != &rhs) {
    this->lex->drop();
    this->lex = rhs.lex;
    this->lex->hold();
    this->datatype = rhs.datatype;
    this->normalized = rhs.normalized;
  }
  return *this;
}

}
