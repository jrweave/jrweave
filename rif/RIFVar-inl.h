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

#include "sys/char.h"

namespace rif {

using namespace ptr;
using namespace std;
using namespace ucs;

template<typename iter>
bool isNCName(iter begin, iter end) {
  if (begin == end || !RIFVar::isNCNameStartChar(*begin)) {
    return false;
  }
  for (++begin; begin != end && RIFVar::isNCNameChar(*begin); ++begin) {
    // look for first invalid character, or end
  }
  return begin == end;
}

inline
RIFVar::RIFVar() throw()
    : utf8name(NULL), normalized(true) {
  // initializers only
}

inline
RIFVar::RIFVar(const RIFVar &copy) throw()
    : utf8name(copy.utf8name), normalized(copy.normalized) {
  if (this->utf8name != NULL) {
    this->utf8name->hold();
  }
}

inline
RIFVar::~RIFVar() throw() {
  if (this->utf8name != NULL) {
    this->utf8name->drop();
  }
}

inline
bool RIFVar::cmplt0(const RIFVar &var1, const RIFVar &var2) throw() {
  return RIFVar::cmp(var1, var2) < 0;
}

inline
bool RIFVar::cmpeq0(const RIFVar &var1, const RIFVar &var2) throw() {
  return RIFVar::cmp(var1, var2) == 0;
}

inline
bool RIFVar::isNCNameStartChar(const uint32_t codepoint) throw() {
  return (to_ascii('A') <= codepoint && codepoint <= to_ascii('Z'))
      || codepoint == to_ascii('_')
      || (to_ascii('a') <= codepoint && codepoint <= to_ascii('z'))
      || (UINT32_C(0xC0) <= codepoint && codepoint <= UINT32_C(0xD6))
      || (UINT32_C(0xD8) <= codepoint && codepoint <= UINT32_C(0xF6))
      || (UINT32_C(0xF8) <= codepoint && codepoint <= UINT32_C(0x2FF))
      || (UINT32_C(0x370) <= codepoint && codepoint <= UINT32_C(0x37D))
      || (UINT32_C(0x37F) <= codepoint && codepoint <= UINT32_C(0x1FFF))
      || (UINT32_C(0x200C) <= codepoint && codepoint <= UINT32_C(0x200D))
      || (UINT32_C(0x2070) <= codepoint && codepoint <= UINT32_C(0x218F))
      || (UINT32_C(0x2C00) <= codepoint && codepoint <= UINT32_C(0x2FEF))
      || (UINT32_C(0x3001) <= codepoint && codepoint <= UINT32_C(0xD7FF))
      || (UINT32_C(0xF900) <= codepoint && codepoint <= UINT32_C(0xFDCF))
      || (UINT32_C(0xFDF0) <= codepoint && codepoint <= UINT32_C(0xFFFD))
      || (UINT32_C(0x10000) <= codepoint && codepoint <= UINT32_C(0xEFFFF));
}

inline
bool RIFVar::isNCNameChar(const uint32_t codepoint) throw() {
  return codepoint == to_ascii('-')
      || codepoint == to_ascii('.')
      || (to_ascii('0') <= codepoint && codepoint <= to_ascii('9'))
      || RIFVar::isNCNameStartChar(codepoint)
      || codepoint == UINT32_C(0xB7)
      || (UINT32_C(0x0300) <= codepoint && codepoint <= UINT32_C(0x036F))
      || (UINT32_C(0x203F) <= codepoint && codepoint <= UINT32_C(0x2040));
}

inline
bool RIFVar::equals(const RIFVar &rhs) const throw() {
  return RIFVar::cmp(*this, rhs) == 0;
}

}
