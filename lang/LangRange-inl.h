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

#include "lang/LangRange.h"

namespace lang {

using namespace std;

template<typename iter>
bool isLanguageRange(iter begin, iter end) {
  if (begin == end) {
    return false;
  }
  if (*begin == to_ascii('*')) {
    return ++begin == end;
  }
  size_t i;
  for (i = 0; i < 8 && begin != end && *begin != to_ascii('-'); ++i) {
    if (!is_alpha(*begin)) {
      return false;
    }
    ++begin;
  }
  if (i < 1) {
    return false;
  }
  if (begin == end) {
    return true;
  }
  if (*begin != to_ascii('-') || ++begin == end) {
    return false;
  }
  for (;;) {
    for (i = 0; i < 8 && begin != end && *begin != to_ascii('-'); ++i) {
      if (!is_alnum(*begin)) {
        return false;
      }
    }
    if (i < 1) {
      return false;
    }
    if (begin == end) {
      return true;
    }
    if (*begin != to_ascii('-') || ++begin == end) {
      return false;
    }
  }
}

template<typename iter>
bool isExtendedLanguageRange(iter begin, iter end) {
  if (begin == end) {
    return false;
  }
  if (*begin == to_ascii('*')) {
    ++begin;
  } else {
    size_t i = 0;
    for (; i < 8 && begin != end && *begin != to_ascii('-'); ++i) {
      if (!is_alpha(*begin)) {
        return false;
      }
      ++begin;
    }
    if (i < 1) {
      return false;
    }
  }
  if (begin == end) {
    return true;
  }
  if (*begin != to_ascii('-') || ++begin == end) {
    return false;
  }
  for (;;) {
    if (*begin == to_ascii('*')) {
      ++begin;
    } else {
      size_t i;
      for (i = 0; i < 8 && begin != end && *begin != to_ascii('-'); ++i) {
        if (!is_alnum(*begin)) {
          return false;
        }
        ++begin;
      }
      if (i < 1) {
        return false;
      }
    }
    if (begin == end) {
      return true;
    }
    if (*begin != to_ascii('-') || ++begin == end) {
      return false;
    }
  }
}

inline
LangRange::LangRange(const LangRange &copy) throw() {
  this->ascii = copy.ascii;
  this->ascii->hold();
}

inline
LangRange::~LangRange() throw() {
  this->ascii->drop();
}

inline
bool LangRange::cmplt0(const LangRange &rng1, const LangRange &rng2) throw() {
  return LangRange::cmp(rng1, rng2) < 0;
}

inline
bool LangRange::cmpeq0(const LangRange &rng1, const LangRange &rng2) throw() {
  return LangRange::cmp(rng1, rng2) == 0;
}

inline
DPtr<uint8_t> *LangRange::getASCIIString() const throw() {
  this->ascii->hold();
  return this->ascii;
}

inline
bool LangRange::equals(const LangRange &rng) const throw() {
  return LangRange::cmp(*this, rng) == 0;
}

inline
bool LangRange::matches(LangTag *lang_tag) const throw() {
  return this->matches(lang_tag, this->isBasic());
}

inline
LangRange &LangRange::operator=(const LangRange &rhs) throw() {
  if (this != &rhs) {
    this->ascii->drop();
    this->ascii = rhs.ascii;
    this->ascii->hold();
  }
  return *this;
}

}
