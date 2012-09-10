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

#include <string>
#include <vector>
#include "ptr/MPtr.h"
#include "sys/ints.h"

#include <iostream>

namespace iri {

using namespace ptr;
using namespace std;

#if 0
template<typename iter>
void debug(const char *label, iter begin, iter end) {
  cerr << label << " ? ";
  for (; begin != end; ++begin) {
    cerr << (char)*begin;
  }
  cerr << endl;
}
#else
#define debug(a, b, c)
#endif

template<typename iter>
bool isIRIReference(iter begin, iter end) {
  debug("isIRIReference", begin, end);
  return isIRI(begin, end) || isIRelativeRef(begin, end);
}

template<typename iter>
bool isIRI(iter begin, iter end) {
  debug("isIRI", begin, end);
  iter mark;
  for (mark = begin; mark != end && *mark != to_ascii(':'); ++mark) {
    // loop does the work
  }
  if (mark == end || !isScheme(begin, mark)) {
    return false;
  }
  ++mark;
  begin = mark;
  for (; mark != end && *mark != to_ascii('?')
      && *mark != to_ascii('#'); ++mark) {
    // loop does the work
  }
  if (!isIHierPart(begin, mark)) {
    return false;
  }
  if (mark == end) {
    return true;
  }
  if (*mark == to_ascii('?')) {
    ++mark;
    begin = mark;
    for (; mark != end && *mark != to_ascii('#'); ++mark) {
      // loop does the work
    }
    if (!isIQuery(begin, mark)) {
      return false;
    }
    if (mark == end) {
      return true;
    }
  }
  ++mark;
  return isIFragment(mark, end);
}

template<typename iter>
bool isIRelativeRef(iter begin, iter end) {
  debug("isRelativeRef", begin, end);
  iter mark;
  for (mark = begin; mark != end && *mark != to_ascii('?')
      && *mark != to_ascii('#'); ++mark) {
    // loop does the work
  }
  if (!isIRelativePart(begin, mark)) {
    return false;
  }
  if (mark == end) {
    return true;
  }
  if (*mark == to_ascii('?')) {
    ++mark;
    begin = mark;
    for (; mark != end && *mark != to_ascii('#'); ++mark) {
      // loop does the work
    }
    if (!isIQuery(begin, mark)) {
      return false;
    }
    if (mark == end) {
      return true;
    }
  }
  ++mark;
  return isIFragment(mark, end);
}

template<typename iter>
bool isScheme(iter begin, iter end) {
  debug("isScheme", begin, end);
  if (begin == end || !is_alpha(*begin)) {
    return false;
  }
  for (++begin; begin != end; ++begin) {
    if (!is_alpha(*begin) && !is_digit(*begin) &&
        *begin != to_ascii('+') && *begin != to_ascii('-') &&
        *begin != to_ascii('.')) {
      return false;
    }
  }
  return true;
}

template<typename iter>
bool isIHierPart(iter begin, iter end) {
  debug("isIHierPart", begin, end);
  iter slash;
  slash = begin;
  if (slash != end && *slash == to_ascii('/')) {
    ++slash;
    if (slash != end && *slash == to_ascii('/')) {
      ++slash;
      iter auth;
      auth = slash;
      for (; slash != end && *slash != to_ascii('/'); ++slash) {
        // loop does the work
      }
      if (isIAuthority(auth, slash) && isIPathAbsEmpty(slash, end)) {
        return true;
      }
    }
  }
  return isIPathAbsolute(begin, end)
      || isIPathRootless(begin, end)
      || isIPathEmpty(begin, end);
}

template<typename iter>
bool isIRelativePart(iter begin, iter end) {
  debug("isIRelativePart", begin, end);
  iter slash;
  slash = begin;
  if (slash != end && *slash == to_ascii('/')) {
    ++slash;
    if (slash != end && *slash == to_ascii('/')) {
      ++slash;
      iter auth;
      auth = slash;
      for (++slash; slash != end &&
          *slash != to_ascii('/'); ++slash) {
        // loop does the work
      }
      if (isIAuthority(auth, slash) && isIPathAbsEmpty(slash, end)) {
        return true;
      }
    }
  }
  return isIPathAbsolute(begin, end)
      || isIPathNoScheme(begin, end)
      || isIPathEmpty(begin, end);
}

template<typename iter>
bool isIAuthority(iter begin, iter end) {
  debug("isIAuthority", begin, end);
  iter mark;
  for (mark = begin; mark != end && *mark != to_ascii('@'); ++mark) {
    // loop does the work
  }
  if (mark == end) {
    mark = begin;
  } else {
      if (!isIUserInfo(begin, mark)) {
        return false;
      }
      ++mark;
      begin = mark;
  }
  if (*begin == to_ascii('[')) {
    for (; mark != end && *mark != to_ascii(']'); ++mark) {
      // loop does the work
    }
    if (mark == end) {
      return false;
    }
    ++mark;
    return isIHost(begin, mark) && (mark == end ||
        (*mark == to_ascii(':') && isPort(++mark, end)));
  }
  for (; mark != end && *mark != to_ascii(':'); ++mark) {
    // loop does the work
  }
  return isIHost(begin, mark) && (mark == end || isPort(++mark, end));
}

template<typename iter>
bool isIUserInfo(iter begin, iter end) {
  debug("isIUserInfo", begin, end);
  for(; begin != end; ++begin) {
    if (*begin != to_ascii(':') && !IRIRef::isIUnreserved(*begin) &&
        !IRIRef::isSubDelim(*begin)) {
      iter start = begin;
      if (++begin == end) return false;
      if (++begin == end) return false;
      if (++begin == end) return false;
      if (!isPctEncoded(start, begin)) {
        return false;
      }
    }
  }
  return true;
}

template<typename iter>
bool isIHost(iter begin, iter end) {
  debug("isIHost", begin, end);
  return isIPLiteral(begin, end) || isIPv4Address(begin, end) ||
      isIRegName(begin, end);
}

template<typename iter>
bool isIPLiteral(iter begin, iter end) {
  debug("isIPLiteral", begin, end);
  if (begin == end || *begin != to_ascii('[')) {
    return false;
  }
  ++begin;
  if (begin == end) {
    return false;
  }
  iter start, finish;
  start = begin;
  finish = begin;
  for (++begin; begin != end; ++begin) {
    ++finish;
  }
  if (*finish != to_ascii(']')) {
    return false;
  }
  return isIPv6Address(start, finish) || isIPvFutureAddress(start, finish);
}

template<typename iter>
bool isIPv4Address(iter begin, iter end) {
  debug("isIPv4Address", begin, end);
  if (begin == end) {
    return false;
  }
  vector<uint32_t> part;
  bool last = false;
  size_t nparts = 0;
  for (;;) {
    if (nparts > 4) {
      return false;
    }
    if (begin == end || *begin == to_ascii('.')) {
      bool valid = part.size() > 0 && (
        (part.size() == 1 && is_digit(part[0]))           ||
        (part.size() == 2 && part[0] != to_ascii('0') &&
         is_digit(part[0]) && is_digit(part[1])) ||
        (part.size() == 3 && part[0] == to_ascii('1') &&
         is_digit(part[1]) && is_digit(part[2])) ||
        (part.size() == 3 && part[0] == to_ascii('2') &&
         to_ascii('0') <= part[1] && part[1] <= to_ascii('4') &&
         is_digit(part[2]))                               ||
        (part.size() == 3 && part[0] == to_ascii('2') &&
         part[1] == to_ascii('5') && to_ascii('0') <= part[2] &&
         part[2] <= to_ascii('5'))                                 );
      if (!valid) {
        return false;
      } 
      part.clear();
      ++nparts;
      if (last) {
        return nparts == 4;
      }
    } else {
      part.push_back(*begin);
    }
    ++begin;
    if (begin == end) {
      last = true;
    }
  }
}

template<typename iter>
bool isIPv6Address(iter begin, iter end) {
  debug("isIPv6Address", begin, end);
  if (begin == end) {
    return false;
  }
  vector<uint32_t> part;
  iter start = begin;
  bool found_double_colon = false;
  size_t nparts = 0;
  for (;;) {
    if (nparts > 8) {
      return false;
    }
    if (*begin == to_ascii('.')) {
      return (found_double_colon || nparts == 6)
          && isIPv4Address(start, end);
    } else if (*begin == to_ascii(':')) {
      if (part.empty()) {
        if (found_double_colon) {
          return false;
        }
        if (nparts == 0) {
          if (++begin == end) {
            return false;
          }
          if (*begin != to_ascii(':')) {
            return false;
          }
        }
        found_double_colon = true;
      } else {
        vector<uint32_t>::iterator it;
        for (it = part.begin(); it != part.end(); ++it) {
          if (!is_xdigit(*it)) {
            return false;
          }
        }
        part.clear();
        ++nparts;
        start = begin;
        ++start;
      }
    } else {
      part.push_back(*begin);
    }
    ++begin;
    if (begin == end) {
      if (!part.empty()) {
        vector<uint32_t>::iterator it;
        for (it = part.begin(); it != part.end(); ++it) {
          if (!is_xdigit(*it)) {
            return false;
          }
        }
        ++nparts;
      }
      return found_double_colon || nparts == 8;
    }
  }
}

template<typename iter>
bool isIPvFutureAddress(iter begin, iter end) {
  debug("isIPvFutureAddress", begin, end);
  // "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
  if (begin == end) {
    return false;
  }
  if (*begin != to_ascii('v')) {
    return false;
  }
  ++begin;
  if (begin == end || !is_xdigit(*begin)) {
    return false;
  }
  for (++begin; begin != end && is_xdigit(*begin); ++begin) {
    // loop finds the first non hexdig
  }
  if (begin == end || *begin != to_ascii('.')) {
    return false;
  }
  ++begin;
  if (begin == end) {
    return false;
  }
  for (; begin != end; ++begin) {
    if (*begin != to_ascii(':') &&
        !IRIRef::isUnreserved(*begin) &&
        !IRIRef::isSubDelim(*begin)) {
      return false;
    }
  }
  return true;
}

template<typename iter>
bool isIRegName(iter begin, iter end) {
  debug("isIRegName", begin, end);
  // *( iunreserved / pct-encoded / sub-delims )
  for (; begin != end; ++begin) {
    if (!IRIRef::isIUnreserved(*begin) && !IRIRef::isSubDelim(*begin)) {
      iter start = begin;
      if (++begin == end) return false;
      if (++begin == end) return false;
      iter mark = begin;
      if (!isPctEncoded(start, ++mark)) {
        return false;
      }
    }
  }
  return true;
}

template<typename iter>
bool isPort(iter begin, iter end) {
  debug("isPort", begin, end);
  for (; begin != end; ++begin) {
    if (!is_digit(*begin)) {
      return false;
    }
  }
  return true;
}

template<typename iter>
bool isIPath(iter begin, iter end) {
  debug("isIPath", begin, end);
  return isIPathEmpty(begin, end) ||
      isIPathAbsEmpty(begin, end) || isIPathAbsolute(begin, end) ||
      isIPathNoScheme(begin, end) || isIPathRootless(begin, end);
}

template<typename iter>
bool isIPathAbsEmpty(iter begin, iter end) {
  debug("isIPathAbsEmpty", begin, end);
  if (begin == end) {
    return true;
  }
  if (*begin != to_ascii('/')) {
    return false;
  }
  vector<uint32_t> part;
  for (++begin; begin != end; ++begin) {
    if (*begin == to_ascii('/')) {
      if (!isISegment(part.begin(), part.end())) {
        return false;
      }
      part.clear();
    } else {
      part.push_back(*begin);
    }
  }
  return isISegment(part.begin(), part.end());
}

template<typename iter>
bool isIPathAbsolute(iter begin, iter end) {
  debug("isIPathAbsolute", begin, end);
  if (begin == end || *begin != to_ascii('/')) {
    return false;
  }
  ++begin;
  return begin == end || isIPathRootless(begin, end);
}

template<typename iter>
bool isIPathNoScheme(iter begin, iter end) {
  debug("isIPathNoScheme", begin, end);
  // isegment-nz-nc *( "/" isegment )
  iter start = begin;
  for (; begin != end && *begin != to_ascii('/'); ++begin) {
    // loop does the work
  }
  return isISegmentNZNC(start, begin) && isIPathAbsEmpty(begin, end);
}

template<typename iter>
bool isIPathRootless(iter begin, iter end) {
  debug("isIPathRootless", begin, end);
  iter start = begin;
  for (; begin != end && *begin != to_ascii('/'); ++begin) {
    // loop does the work
  }
  return isISegmentNZ(start, begin) && isIPathAbsEmpty(begin, end);
}

template<typename iter>
bool isIPathEmpty(iter begin, iter end) {
  debug("isIPathEmpty", begin, end);
  return begin == end;
}

template<typename iter>
bool isISegment(iter begin, iter end) {
  debug("isISegment", begin, end);
  for (; begin != end; ++begin) {
    if (!IRIRef::isIPChar(*begin)) {
      return false;
    }
  }
  return true;
}

template<typename iter>
bool isISegmentNZ(iter begin, iter end) {
  debug("isISegmentNZ", begin, end);
  return begin != end && isISegment(begin, end);
}

template<typename iter>
bool isISegmentNZNC(iter begin, iter end) {
  debug("isISegmentNZNC", begin, end);
  // 1*( iunreserved / pct-encoded / sub-delims / "@" )
  if (begin == end) {
      return false;
  }
  for (; begin != end; ++begin) {
    if (*begin != to_ascii('@') && !IRIRef::isIUnreserved(*begin) &&
        !IRIRef::isSubDelim(*begin)) {
      iter start = begin;
      if (++begin == end) return false;
      if (++begin == end) return false;
      if (++begin == end) return false;
      if (!isPctEncoded(start, begin)) {
        return false;
      }
    }
  }
  return true;
}

template<typename iter>
bool isIQuery(iter begin, iter end) {
  debug("isIQuery", begin, end);
  // *( ipchar / iprivate / "/" / "?" )
  for (; begin != end; ++begin) {
    if (*begin != to_ascii('/') && *begin != to_ascii('?') &&
        !IRIRef::isIPChar(*begin) && !IRIRef::isIPrivate(*begin)) {
      return false;
    }
  }
  return true;
}

template<typename iter>
bool isIFragment(iter begin, iter end) {
  debug("isIFragment", begin, end);
  // *( ipchar / "/" / "?" )
  for (; begin != end; ++begin) {
    if (*begin != to_ascii('/') && *begin != to_ascii('?') &&
        !IRIRef::isIPChar(*begin)) {
      return false;
    }
  }
  return true;
}

template<typename iter>
bool isPctEncoded(iter begin, iter end) {
  debug("isPctEncoded", begin, end);
  if (begin == end || *begin != to_ascii('%')) {
    return false;
  }
  ++begin;
  if (begin == end || !is_xdigit(*begin)) {
    return false;
  }
  ++begin;
  return begin != end && is_xdigit(*begin);
}

inline
IRIRef::IRIRef() throw(BadAllocException)
    : normalized(false), urified(false) {
  try {
    NEW(this->utf8str, MPtr<uint8_t>);
  } JUST_RETHROW(BadAllocException, "(rethrow)")
}

inline
IRIRef::IRIRef(const IRIRef &iri) throw()
    : normalized(iri.normalized), urified(iri.urified), utf8str(iri.utf8str) {
  this->utf8str->hold();
}

inline
IRIRef::IRIRef(const IRIRef *iri) throw()
    : normalized(iri->normalized), urified(iri->urified),
      utf8str(iri->utf8str) {
  this->utf8str->hold();
}

inline
IRIRef::~IRIRef() throw() {
  if (this->utf8str != NULL) {
    this->utf8str->drop();
  }
}

inline
bool IRIRef::cmplt0(const IRIRef &ref1, const IRIRef &ref2) throw() {
  return IRIRef::cmp(ref1, ref2) < 0;
}

inline
bool IRIRef::cmpeq0(const IRIRef &ref1, const IRIRef &ref2) throw() {
  return IRIRef::cmp(ref1, ref2) == 0;
}

inline
bool IRIRef::isIPChar(const uint32_t codepoint) throw() {
  return codepoint == to_ascii(':')
    || codepoint == to_ascii('@')
    || codepoint == to_ascii('%') // for pct-encoded
    || IRIRef::isIUnreserved(codepoint)
    || IRIRef::isSubDelim(codepoint);
}

inline
bool IRIRef::isReserved(const uint32_t codepoint) throw() {
  return IRIRef::isGenDelim(codepoint)
    || IRIRef::isSubDelim(codepoint);
}

inline
bool IRIRef::isUnreserved(const uint32_t codepoint) throw() {
  return is_alpha(codepoint)
    || is_digit(codepoint)
    || codepoint == to_ascii('-')
    || codepoint == to_ascii('.')
    || codepoint == to_ascii('_')
    || codepoint == to_ascii('~');
}

inline
bool IRIRef::isIUnreserved(const uint32_t codepoint) throw() {
  return IRIRef::isUnreserved(codepoint)
    || IRIRef::isUCSChar(codepoint);
}

inline
bool IRIRef::isUCSChar(const uint32_t codepoint) throw() {
  return (UINT32_C(0xA0) <= codepoint && codepoint <= UINT32_C(0xD7FF))
    || (UINT32_C(0xF900) <= codepoint && codepoint <= UINT32_C(0xFDCF))
    || (UINT32_C(0xFDF0) <= codepoint && codepoint <= UINT32_C(0xFFEF))
    || (UINT32_C(0x10000) <= codepoint && codepoint <= UINT32_C(0x1FFFD))
    || (UINT32_C(0x20000) <= codepoint && codepoint <= UINT32_C(0x2FFFD))
    || (UINT32_C(0x30000) <= codepoint && codepoint <= UINT32_C(0x3FFFD))
    || (UINT32_C(0x40000) <= codepoint && codepoint <= UINT32_C(0x4FFFD))
    || (UINT32_C(0x50000) <= codepoint && codepoint <= UINT32_C(0x5FFFD))
    || (UINT32_C(0x60000) <= codepoint && codepoint <= UINT32_C(0x6FFFD))
    || (UINT32_C(0x70000) <= codepoint && codepoint <= UINT32_C(0x7FFFD))
    || (UINT32_C(0x80000) <= codepoint && codepoint <= UINT32_C(0x8FFFD))
    || (UINT32_C(0x90000) <= codepoint && codepoint <= UINT32_C(0x9FFFD))
    || (UINT32_C(0xA0000) <= codepoint && codepoint <= UINT32_C(0xAFFFD))
    || (UINT32_C(0xB0000) <= codepoint && codepoint <= UINT32_C(0xBFFFD))
    || (UINT32_C(0xC0000) <= codepoint && codepoint <= UINT32_C(0xCFFFD))
    || (UINT32_C(0xD0000) <= codepoint && codepoint <= UINT32_C(0xDFFFD))
    || (UINT32_C(0xE1000) <= codepoint && codepoint <= UINT32_C(0xEFFFD));
}

inline
bool IRIRef::isSubDelim(const uint32_t codepoint) throw() {
  return codepoint == to_ascii('!')
    || codepoint == to_ascii('$')
    || codepoint == to_ascii('&')
    || codepoint == to_ascii('\'')
    || codepoint == to_ascii('(')
    || codepoint == to_ascii(')')
    || codepoint == to_ascii('*')
    || codepoint == to_ascii('+')
    || codepoint == to_ascii(',')
    || codepoint == to_ascii(';')
    || codepoint == to_ascii('=');
}

inline
bool IRIRef::isGenDelim(const uint32_t codepoint) throw() {
  return codepoint == to_ascii(':')
    || codepoint == to_ascii('/')
    || codepoint == to_ascii('?')
    || codepoint == to_ascii('#')
    || codepoint == to_ascii('[')
    || codepoint == to_ascii(']')
    || codepoint == to_ascii('@');
}

inline
bool IRIRef::isIPrivate(const uint32_t codepoint) throw() {
  return (UINT32_C(0xE000) <= codepoint && codepoint <= UINT32_C(0xF8FF))
    || (UINT32_C(0xF0000) <= codepoint && codepoint <= UINT32_C(0xFFFFD))
    || (UINT32_C(0x100000) <= codepoint && codepoint <= UINT32_C(0x10FFFD));
}

inline
DPtr<uint8_t> *IRIRef::getUTF8String() const throw() {
  this->utf8str->hold();
  return this->utf8str;
}

inline
bool IRIRef::equals(const IRIRef &ref) const throw() {
  return IRIRef::cmp(*this, ref) == 0;
}

inline
bool IRIRef::isIRI() const throw() {
  return !this->isRelativeRef();
}

inline
IRIRef &IRIRef::operator=(const IRIRef &rhs) throw() {
  this->utf8str->drop();
  this->utf8str = rhs.utf8str;
  this->utf8str->hold();
  this->normalized = rhs.normalized;
  this->urified = rhs.urified;
}

}
