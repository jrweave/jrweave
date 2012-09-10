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

#include "ptr/alloc.h"
#include "ptr/MPtr.h"

namespace lang {

using namespace ex;
using namespace ptr;
using namespace std;

LangRange::LangRange() THROWS(BadAllocException) {
  NEW(this->ascii, MPtr<uint8_t>, 1);
  (*this->ascii)[0] = to_ascii('*');
}
TRACE(BadAllocException, "(trace)")

LangRange::LangRange(DPtr<uint8_t> *ascii) throw(MalformedLangRangeException) {
  if (!isExtendedLanguageRange(ascii->dptr(), ascii->dptr() + ascii->size())) {
    THROW(MalformedLangRangeException, ascii);
  }
  this->ascii = ascii;
  this->ascii->hold();
}

int LangRange::cmp(const LangRange &rng1, const LangRange &rng2) throw() {
  if (&rng1 == &rng2) {
    return 0;
  }
  size_t minlen = min(rng1.ascii->size(), rng2.ascii->size());
  uint8_t *begin = rng1.ascii->dptr();
  uint8_t *end = begin + minlen;
  uint8_t *mark = rng2.ascii->dptr();
  for (; begin != end; ++begin) {
    if (to_lower(*begin) != to_lower(*mark)) {
      return to_lower(*begin) < to_lower(*mark) ? -1 : 1;
    }
    ++mark;
  }
  return rng1.ascii->size() < rng2.ascii->size() ? -1 :
        (rng1.ascii->size() > rng2.ascii->size() ?  1 : 0);
}

bool LangRange::isBasic() const throw() {
  uint8_t *begin = this->ascii->dptr();
  uint8_t *end = begin + this->ascii->size();
  for (; begin != end; ++begin) {
    if (*begin == to_ascii('*')) {
      return this->ascii->size() == 1;
    }
  }
  return true;
}

bool LangRange::matches(LangTag *lang_tag, bool basic) const throw() {
  if (this->ascii->size() == 1 && (*(this->ascii))[0] == to_ascii('*')) {
    return true;
  }

  DPtr<uint8_t> *tag = lang_tag->getASCIIString();

  if (basic) {
    if (tag->size() < this->ascii->size()) {
      tag->drop();
      return false;
    }
    uint8_t *rbegin = this->ascii->dptr();
    uint8_t *rend = rbegin + this->ascii->size();
    uint8_t *tbegin = tag->dptr();
    uint8_t *tend = tbegin + tag->size();
    for (; rbegin != rend; ++rbegin) {
      if (to_lower(*rbegin) != to_lower(*tbegin)) {
        tag->drop();
        return false;
      }
      ++tbegin;
    }
    bool ret = tbegin == tend || *tbegin == to_ascii('-');
    tag->drop();
    return ret;
  }

  uint8_t *rbegin = this->ascii->dptr();
  uint8_t *rmark = rbegin;
  uint8_t *rend = rbegin + this->ascii->size();
  for (; rmark != rend && *rmark != to_ascii('-'); ++rmark) {
    // loop finds hyphen or end
  }

  uint8_t *tbegin = tag->dptr();
  uint8_t *tmark = tbegin;
  uint8_t *tend = tbegin + tag->size();
  for (; tmark != tend && *tmark != to_ascii('-'); ++tmark) {
    // loop finds hyphen or end
  }

  if (*rbegin == to_ascii('*')) {
    rbegin = rmark;
    tbegin = tmark;
  } else {
    if (rmark - rbegin != tmark - tbegin) {
      tag->drop();
      return false;
    }

    for (; rbegin != rmark; ++rbegin) {
      if (to_lower(*rbegin) != to_lower(*tbegin)) {
        return false;
      }
      ++tbegin;
    }
  }
  if (rbegin != rend) {
    rbegin = ++rmark;
    for (; rmark != rend && *rmark != to_ascii('-'); ++rmark) {
      // loop finds hyphen or end
    }
  }
  if (tbegin != tend) {
    tbegin = ++tmark;
    for (; tmark != tend && *tmark != to_ascii('-'); ++tmark) {
      // loop finds hyphen or end
    }
  }
  while (rbegin != rend) {
    if (*rbegin == to_ascii('*')) {
      if (rmark == rend) {
        rbegin = rend;
      } else {
        rbegin = ++rmark;
        for (; rmark != rend && *rmark != to_ascii('-'); ++rmark) {
          // loop finds hyphen or end
        }
      }
      continue;
    }
    if (tbegin == tend) {
      tag->drop();
      return false;
    }
    if (rmark - rbegin == tmark - tbegin) {
      for (; rbegin != rmark; ++rbegin) {
        if (to_lower(*rbegin) != to_lower(*tbegin)) {
          break;
        }
        ++tbegin;
      }
      if (rmark == rend) {
        rbegin = rend;
      } else {
        rbegin = ++rmark;
        for (; rmark != rend && *rmark != to_ascii('-'); ++rmark) {
          // loop finds hyphen or end
        }
      }
      if (tmark == tend) {
        tbegin = tend;
      } else {
        tbegin = ++tmark;
        for (; tmark != tend && *tmark != to_ascii('-'); ++tmark) {
          // loop finds hyphen or end
        }
      }
      continue;
    }
    if (tmark - tbegin == 1) {
      tag->drop();
      return false;
    }
    if (tmark == tend) {
      tbegin = tend;
    } else {
      tbegin = ++tmark;
      for (; tmark != tend && *tmark != to_ascii('-'); ++tmark) {
        // loop finds hyphen or end
      }
    }
  }
  tag->drop();
  return true;
}

} // end namespace lang

std::istream& operator>>(std::istream& stream, lang::LangRange &rng) {
  std::string str;
  stream >> str;
  ptr::DPtr<uint8_t> *p;
  try {
    NEW(p, ptr::MPtr<uint8_t>, str.size());
  } RETHROW_BAD_ALLOC
  ascii_strcpy(p->dptr(), str.c_str());
  try {
    lang::LangRange newrng(p);
    p->drop();
    rng = newrng;
    return stream;
  } catch(lang::MalformedLangRangeException &e) {
    p->drop();
    RETHROW(e, "Invalid std::istream >> lang::LangRange operation.");
  }
}

std::ostream& operator<<(std::ostream& stream, const lang::LangRange &rng) {
  ptr::DPtr<uint8_t> *asciistr = rng.getASCIIString();
  const uint8_t *begin = asciistr->dptr();
  const uint8_t *end = begin + asciistr->size();
  for (; begin != end; ++begin) {
    stream << to_lchar(*begin);
  }
  asciistr->drop();
  return stream;
}
