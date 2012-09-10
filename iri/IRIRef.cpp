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

#include "iri/IRIRef.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include "iri/MalformedIRIRefException.h"
#include "ptr/BadAllocException.h"
#include "ptr/DPtr.h"
#include "ptr/MPtr.h"
#include "ptr/OPtr.h"
#include "sys/ints.h"
#include "ucs/nf.h"
#include "ucs/UTF8Iter.h"

#ifdef UCS_NO_C
#error "The IRIRef class depends on NFC normalization, thus UCS_NO_C should not be defined.\n"
#endif

namespace iri {

using namespace ptr;
using namespace std;
using namespace ucs;

IRIRef::IRIRef(DPtr<uint8_t> *utf8str)
    throw(SizeUnknownException, MalformedIRIRefException,
          InvalidEncodingException, InvalidCodepointException)
    : normalized(false), urified(false), utf8str(NULL) {
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  try {
    utf8validate(utf8str);
  } JUST_RETHROW(InvalidEncodingException, "(rethrow)")
    JUST_RETHROW(InvalidCodepointException, "(rethrow)")
  UTF8Iter begin (utf8str);
  UTF8Iter end (utf8str);
  end.finish();
  if (!isIRIReference(begin, end)) {
      UTF8Iter *iter;
      NEW(iter, UTF8Iter, utf8str);
      THROW(MalformedIRIRefException, iter);
  }
  this->utf8str = utf8str;
  this->utf8str->hold();
}

int IRIRef::cmp(const IRIRef &ref1, const IRIRef &ref2) throw() {
  if (&ref1 == &ref2) {
    return 0;
  }
  size_t len = min(ref1.utf8str->size(), ref2.utf8str->size());
  int cmp = memcmp(ref1.utf8str->dptr(), ref2.utf8str->dptr(),
      len * sizeof(uint8_t));
  if (cmp != 0) {
    return cmp;
  }
  return ref1.utf8str->size() < ref2.utf8str->size() ? -1 :
        (ref1.utf8str->size() > ref2.utf8str->size() ?  1 : 0);
}

bool IRIRef::isRelativeRef() const throw() {
  DPtr<uint8_t> *scheme = this->getPart(SCHEME);
  if (scheme == NULL) {
    return true;
  }
  scheme->drop();
  return false;
}

bool IRIRef::isAbsoluteIRI() const throw() {
  if (this->isRelativeRef()) {
    return false;
  }
  DPtr<uint8_t> *fragment = this->getPart(FRAGMENT);
  if (fragment == NULL) {
    return true;
  }
  fragment->drop();
  return false;
}

DPtr<uint8_t> *IRIRef::getPart(const enum IRIRefPart part) const throw() {

  const uint8_t *begin = this->utf8str->dptr();
  const uint8_t *end = begin + this->utf8str->size();
  const uint8_t *offset = begin;
  const uint8_t *mark = begin;
  DPtr<uint8_t> *iripart = NULL;

  // SCHEME
  for (; mark != end && *mark != to_ascii(':'); ++mark) {
    // loop does the work
  }
  if (mark == end) {
    if (part == SCHEME) {
      return NULL;
    }
    // offset unchanged
  } else {
    iripart = this->utf8str->sub(0, mark - offset);
    UTF8Iter begin (iripart);
    UTF8Iter end (iripart);
    end.finish();
    if (isScheme(begin, end)) {
      if (part == SCHEME) {
        return iripart;
      }
      offset = mark + 1;
    } else {
      if (part == SCHEME) {
        iripart->drop();
        return NULL;
      }
      // offset unchanged
    }
    iripart->drop();
  }

  // These parts exist only if hierarchy starts with //.
  if (part == USER_INFO || part == HOST || part == PORT) {
    if (offset == end || *offset != to_ascii('/') ||
        ++offset == end || *offset != to_ascii('/')) {
      return NULL;
    }
    ++offset;

    // USER_INFO
    for (mark = offset;
         mark != end && *mark != to_ascii('@') && *mark != to_ascii('/');
         ++mark) {
      // loop does the work
    }
    if (mark == end || *mark == to_ascii('/')) {
      if (part == USER_INFO) {
        return NULL;
      }
      // offset unchanged
    } else {
      if (part == USER_INFO) {
        return this->utf8str->sub(offset - begin, mark - offset);
      }
      offset = mark + 1;
    }

    // HOST
    if (offset != end && *offset == to_ascii('[')) {
      for (mark = offset;
           mark != end && *mark != to_ascii(']');
           ++mark) {
        // loop does the work
      }
      ++mark;
    } else {
      for (mark = offset;
           mark != end && *mark != to_ascii(':') && *mark != to_ascii('/') &&
           *mark != to_ascii('?') && *mark != to_ascii('#');
           ++mark) {
        // loop does the work
      }
    }
    if (part == HOST) {
      return this->utf8str->sub(offset - begin, mark - offset);
    }
    offset = mark;
    // if a colon was found, include it to help distinguish
    // between no port and empty port

    // PORT
    // No need to check part == PORT.  It is definitely PORT
    // by process of elimination.
    if (offset == end || *offset != to_ascii(':')) {
      return NULL;
    }
    if (*offset == to_ascii(':')) {
      offset++;
      for (mark = offset; mark != end && *mark != to_ascii('/') &&
           *mark != to_ascii('?') && *mark != to_ascii('#'); ++mark) {
        // loop does the work
      }
      return this->utf8str->sub(offset - begin, mark - offset);
    }
  }
  
  // Skip over //authority if necessary.
  if (end - offset >= 2 && *offset == to_ascii('/')
      && *(offset + 1) == to_ascii('/')) {
    for (offset += 2; offset != end && *offset != to_ascii('/')
         && *offset != to_ascii('?') && *offset != to_ascii('#'); ++offset) {
      // loop does the work
    }
  }

  // PATH
  for (mark = offset;
       mark != end && *mark != to_ascii('?') && *mark != to_ascii('#');
       ++mark) {
    // loop does the work
  }
  if (part == PATH) {
    return this->utf8str->sub(offset - begin, mark - offset);
  }
  offset = mark;
  // leave ? or # for reference

  // QUERY
  if (offset == end) {
    return NULL;
  }
  if (*offset == to_ascii('?')) {
    ++offset;
    for (mark = offset; mark != end && *mark != to_ascii('#'); ++mark) {
      // loop does the work
    }
    if (part == QUERY) {
      return this->utf8str->sub(offset - begin, mark - offset);
    }
    offset = mark;
    // leave # for reference
  } else {
    if (part == QUERY) {
      return NULL;
    }
  }

  // FRAGMENT
  if (offset == end || *offset != to_ascii('#')) {
    return NULL;
  }
  ++offset;
  return this->utf8str->sub(offset - begin, end - offset);
}

IRIRef *IRIRef::normalize() THROWS(BadAllocException, TraceableException) {
  if (this->normalized && !this->urified) {
    return this;
  }

  uint8_t *normed = NULL;
  DPtr<uint8_t> *normal = NULL;

  if (this->utf8str->standable()) {
    this->utf8str = this->utf8str->stand();
    normal = this->utf8str;
    normal->hold();
  } else {
    NEW(normal, MPtr<uint8_t>, this->utf8str->size());
  }
  normed = normal->dptr();

  // Percent encoding; decode if IUnreserved
  uint8_t bits = UINT8_C(0);
  uint8_t *begin = this->utf8str->dptr();
  uint8_t *end = begin + this->utf8str->size();
  uint8_t *marki = begin;
  uint8_t *markj = begin;
  uint8_t *markk = normed;
  for (; marki != end; marki = markj) {
    for (markj = marki; markj != end && *markj != to_ascii('%'); ++markj) {
      // loop does the work
    }
    size_t sz = markj - marki;
    memmove(markk, marki, sz * sizeof(uint8_t));
    markk += sz;
    if (markj == end) {
      break;
    }
    size_t i;
    for (i = 0; bits > UINT8_C(0x7F) && i < sz; ++i) {
      bits <<= 1;
    }
    // decode percent-encoding
    uint8_t n = (((uint8_t) IRI_HEX_VALUE(markj[1])) << 4)
        | (uint8_t) IRI_HEX_VALUE(markj[2]);
    if (bits > UINT8_C(0x7F) || IRIRef::isIUnreserved(n)) {
      if (bits > UINT8_C(0x7F)) {
        bits <<= 1;
      } else if (n > UINT8_C(0x7F)) {
        bits = n << 1;
      }
      *markk = n;
      ++markk;
      markj += 3;
      if (this->urified && IRIRef::isUCSChar(n)) {
        this->urified = false;
      }
    } else {
      if (bits > UINT8_C(0x7F)) {
        bits <<= 1;
      }
      markk[0] = markj[0];
      markk[1] = to_upper(markj[1]);
      markk[2] = to_upper(markj[2]);
      markj += 3;
      markk += 3;
    }
  }
  if (markk != end) {
    DPtr<uint8_t> *temp = normal->sub(0, markk - normed);
    normal->drop();
    normal = temp;
  }

  if (this->normalized) {
    this->utf8str->drop();
    this->utf8str = normal;
    return this;
  }

  // Character normalization; NFC
  DPtr<uint32_t> *codepoints = utf8dec(normal);
  normal->drop();
  DPtr<uint32_t> *codepoints2 = nfc_opt(codepoints);
  codepoints->drop();
  normal = utf8enc(codepoints2);
  codepoints2->drop();
  this->utf8str->drop();
  this->utf8str = normal;

  // Path normalization
  this->resolve(NULL);

  // Case normalization; scheme and host
  DPtr<uint8_t> *part = this->getPart(SCHEME);
  if (part != NULL) {
    begin = part->dptr();
    end = begin + part->size();
    for (; begin != end; ++begin) {
      *begin = to_lower(*begin);
    }
    part->drop();
  }
  part = this->getPart(HOST);
  if (part != NULL) {
    begin = part->dptr();
    end = begin + part->size();
    for (; begin != end; ++begin) {
      *begin = to_lower(*begin);
    }
    part->drop();
  }

  this->normalized = true;
  return this;
}
TRACE(BadAllocException, "(trace)")

IRIRef *IRIRef::urify() throw(BadAllocException) {
  if (this->urified) {
    return this;
  }
  const uint8_t *begin = this->utf8str->dptr();
  const uint8_t *end = begin + this->utf8str->size();
  const uint8_t *mark = begin;
  const uint8_t *next = NULL;
  uint32_t codepoint;
  while (mark != end) {
    codepoint = utf8char(mark, &next);
    if (IRIRef::isIPrivate(codepoint) || IRIRef::isUCSChar(codepoint)) {
      break;
    }
    mark = next;
  }
  if (mark == end) {
    this->urified = true;
    return this;
  }
  vector<uint8_t> urivec;
  urivec.reserve(this->utf8str->size() + 2);
  do {
    urivec.insert(urivec.end(), begin, mark);
    for (; mark != next; ++mark) {
      urivec.push_back(to_ascii('%'));
      uint8_t hi4 = (*mark) >> 4;
      uint8_t lo4 = (*mark) & UINT8_C(0x0F);
      urivec.push_back(hi4 <= UINT8_C(9) ? to_ascii('0') + hi4
                                         : to_ascii('A') + (hi4 - 10));
      urivec.push_back(lo4 <= UINT8_C(9) ? to_ascii('0') + lo4
                                         : to_ascii('A') + (lo4 - 10));
    }
    begin = mark;
    while (mark != end) {
      codepoint = utf8char(mark, &next);
      if (IRIRef::isIPrivate(codepoint) || IRIRef::isUCSChar(codepoint)) {
        break;
      }
      mark = next;
    }
  } while(mark != end);
  urivec.insert(urivec.end(), begin, mark);
  MPtr<uint8_t> *uristr;
  try {
    NEW(uristr, MPtr<uint8_t>, urivec.size());
  } RETHROW_BAD_ALLOC
  copy(urivec.begin(), urivec.end(), uristr->dptr());
  this->utf8str->drop();
  this->utf8str = uristr;
  this->urified = true;
  return this; 
}

IRIRef *IRIRef::resolve(IRIRef *base) THROWS(BadAllocException) {
  // Equivalent to remove_dot_segments
  if (base == NULL || base == this) {

    if (this->normalized || this->urified) {
      return this;
    }

    DPtr<uint8_t> *normal = this->getPart(PATH);
    DPtr<uint8_t> *query = this->getPart(QUERY);
    DPtr<uint8_t> *fragment = this->getPart(FRAGMENT);

    uint8_t *begin = normal->dptr();
    uint8_t *end = begin + normal->size();
    uint8_t *marki = begin;
    uint8_t *markj = begin;
    while (marki != end) {
      size_t remaining = end - marki;
      if (remaining >= 3 && ascii_strncmp(marki, "../", 3) == 0) {
        marki += 3;
      } else  if (remaining >= 2 && ascii_strncmp(marki, "./", 2) == 0) {
        marki += 2;
      } else if (remaining >= 3 && ascii_strncmp(marki, "/./", 3) == 0) {
        marki += 2;
      } else if (remaining == 2 && ascii_strncmp(marki, "/.", 2) == 0) {
        *(++marki) = to_ascii('/');
      } else if (remaining >= 4 && ascii_strncmp(marki, "/../", 4) == 0) {
        for (; markj != begin && *(markj - 1) != to_ascii('/'); --markj) {
          // loop does the work
        }
        if (markj != begin) {
          --markj; // get rid of the slash, too
        }
        marki += 3;
      } else if (remaining == 3 && ascii_strncmp(marki, "/..", 3) == 0) {
        for (; markj != begin && *(markj - 1) != to_ascii('/'); --markj) {
          // loop does the work
        }
        if (markj != begin) {
          --markj; // get rid of the slash, too
        }
        marki += 2;
        *marki = to_ascii('/');
      } else if (remaining == 1 && *marki == to_ascii('.')) {
        ++marki;
      } else if (remaining == 2 && ascii_strncmp(marki, "..", 2) == 0) {
        marki += 2;
      } else {
        uint8_t *markk = marki + 1;
        for (; markk != end && *markk != to_ascii('/'); ++markk) {
          // loop does the work
        }
        memmove(markj, marki, (markk - marki) * sizeof(uint8_t));
        markj += (markk - marki);
        marki = markk;
      }
    }
    if (query != NULL) {
      memmove(markj, query->dptr() - 1,
          (query->size() + 1) * sizeof(uint8_t));
      markj += query->size() + 1;
      query->drop();
    }
    if (fragment != NULL) {
      memmove(markj, fragment->dptr() - 1,
          (fragment->size() + 1) * sizeof(uint8_t));
      markj += fragment->size() + 1;
      fragment->drop();
    }
    fragment = this->utf8str->sub(0, markj - this->utf8str->dptr());
    normal->drop();
    this->utf8str->drop();
    this->utf8str = fragment;
    return this;
  }

  DPtr<uint8_t> *scheme = this->getPart(SCHEME);
  if (scheme != NULL) {
    scheme->drop();
    return this->resolve(NULL);
  }

  DPtr<uint8_t> *user_info = NULL;
  DPtr<uint8_t> *host = this->getPart(HOST);
  DPtr<uint8_t> *port = NULL;
  DPtr<uint8_t> *path = NULL;
  DPtr<uint8_t> *query = NULL;
  DPtr<uint8_t> *fragment = NULL;
  if (host != NULL) {
    user_info = this->getPart(USER_INFO);
    port = this->getPart(PORT);
    path = this->getPart(PATH);
    query = this->getPart(QUERY);
  } else {
    path = this->getPart(PATH);
    if (path->size() == 0) {
      path->drop();
      path = base->getPart(PATH);
      query = this->getPart(QUERY);
      if (query == NULL) {
        query = base->getPart(QUERY);
      }
    } else {
      if ((*path)[0] == (uint8_t) to_ascii('/')) {
        path->drop();
        path = this->getPart(PATH);
      } else {
        // BEGIN merge
        DPtr<uint8_t> *base_host = base->getPart(HOST);
        DPtr<uint8_t> *base_path = base->getPart(PATH);
        if (base_host != NULL && base_path->size() > 0) {
          DPtr<uint8_t> *newpath;
          NEW(newpath, MPtr<uint8_t>, path->size() + 1);
          (*newpath)[0] = (uint8_t) to_ascii('/');
          memcpy(newpath->dptr() + 1, path->dptr(),
              path->size() * sizeof(uint8_t));
          path->drop();
          path = newpath;
        } else {
          const uint8_t *begin = base_path->dptr();
          const uint8_t *end = begin + base_path->size();
          const uint8_t *slash = end;
          for (; slash != begin && *(slash-1) != to_ascii('/'); --slash) {
            // loop does the work; finds last slash in base path
          }
          DPtr<uint8_t> *newpath;
          size_t sz = slash - begin;
          NEW(newpath, MPtr<uint8_t>, sz + path->size());
          memcpy(newpath->dptr(), begin, sz * sizeof(uint8_t));
          memcpy(newpath->dptr() + sz, path->dptr(),
              path->size() * sizeof(uint8_t));
          path->drop();
          path = newpath;
        }
        if (base_host != NULL) {
          base_host->drop();
        }
        base_path->drop();
        // END merge
      }
      query = this->getPart(QUERY);
    }
    user_info = this->getPart(USER_INFO);
    host = this->getPart(HOST);
    port = this->getPart(PORT);
  }
  scheme = base->getPart(SCHEME);
  fragment = this->getPart(FRAGMENT);

  size_t len = (scheme == NULL ? 0 : scheme->size() + 1)
      + (user_info == NULL ? 0 : user_info->size() + 1)
      + (host == NULL ? 0 : host->size() + 2)
      + (port == NULL ? 0 : port->size() + 1)
      + path->size()
      + (query == NULL ? 0 : query->size() + 1)
      + (fragment == NULL ? 0 : fragment->size() + 1);
  uint8_t *iristr = NULL;
  alloc(iristr, len);
  if (iristr != NULL) {
    len = 0;
    if (scheme != NULL) {
      memcpy(iristr + len, scheme->dptr(), scheme->size() * sizeof(uint8_t));
      len += scheme->size();
      iristr[len++] = (uint8_t) to_ascii(':');
    }
    if (host != NULL) {
      iristr[len++] = (uint8_t) to_ascii('/');
      iristr[len++] = (uint8_t) to_ascii('/');
      if (user_info != NULL) {
        memcpy(iristr + len, user_info->dptr(),
            user_info->size() * sizeof(uint8_t));
        len += user_info->size();
        iristr[len++] = (uint8_t) to_ascii('@');
      }
      memcpy(iristr + len, host->dptr(), host->size() * sizeof(uint8_t));
      len += host->size();
      if (port != NULL) {
        iristr[len++] = (uint8_t) to_ascii(':');
        memcpy(iristr + len, port->dptr(), port->size() * sizeof(uint8_t));
        len += port->size();
      }
    }
    memcpy(iristr + len, path->dptr(), path->size() * sizeof(uint8_t));
    len += path->size();
    if (query != NULL) {
      iristr[len++] = (uint8_t) to_ascii('?');
      memcpy(iristr + len, query->dptr(), query->size() * sizeof(uint8_t));
      len += query->size();
    }
    if (fragment != NULL) {
      iristr[len++] = (uint8_t) to_ascii('#');
      memcpy(iristr + len, fragment->dptr(),
          fragment->size() * sizeof(uint8_t));
      len += fragment->size();
    }
  }
  if (scheme != NULL) scheme->drop();
  if (user_info != NULL) user_info->drop();
  if (host != NULL) host->drop();
  if (port != NULL) port->drop();
  path->drop();
  if (query != NULL) query->drop();
  if (fragment != NULL) fragment->drop();
  if (iristr == NULL) {
    THROW(BadAllocException, len * sizeof(uint8_t));
  }
  this->utf8str->drop();
  NEW(this->utf8str, MPtr<uint8_t>, iristr, len);

  return this->resolve(NULL);
}
TRACE(BadAllocException, "(trace)")

} // end namespace iri

std::istream& operator>>(std::istream& stream, iri::IRIRef &ref) {
  std::string str;
  stream >> str;
  ptr::DPtr<uint8_t> *p;
  try {
    NEW(p, ptr::MPtr<uint8_t>, str.size());
  } RETHROW_BAD_ALLOC
  ascii_strcpy(p->dptr(), str.c_str());
  try {
    iri::IRIRef newref(p);
    p->drop();
    ref = newref;
    return stream;
  } catch(iri::MalformedIRIRefException &e) {
    p->drop();
    RETHROW(e, "Invalid std::istream >> iri::IRIRef operation.");
  }
}

std::ostream& operator<<(std::ostream& stream, const iri::IRIRef &ref) {
  ptr::DPtr<uint8_t> *utf8str = ref.getUTF8String();
  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  for (; begin != end; ++begin) {
    stream << to_lchar(*begin);
  }
  utf8str->drop();
  return stream;
}
