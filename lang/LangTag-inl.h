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

#include "lang/LangTag.h"

#include <cctype>
#include <iostream>
#include "sys/char.h"

namespace lang {

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
inline
bool isLanguageTag(iter begin, iter end) {
  return isLangTag(begin, end)
      || isPrivateUse(begin, end)
      || isGrandfathered(begin, end);
}

template<typename iter>
bool isLangTag(iter begin, iter end) {

  iter m1, m2;
  for (m1 = begin; m1 != end && *m1 != to_ascii('-'); ++m1) {
    // loop does the work
  }

  // language
  if (!isLanguage(begin, m1)) {
    return false;
  }
  m2 = m1;
  do {
    if (m2 == end) {
      return true;
    }
    m1 = m2;
    for (++m2; m2 != end && *m2 != to_ascii('-'); ++m2) {
      // loop does the work
    }
  } while (isLanguage(begin, m2));
  if (++m1 == end) {
    return false;
  }
  begin = m1;
  for (; m1 != end && *m1 != to_ascii('-'); ++m1) {
    // loop does the work
  }
  
  // script
  if (isScript(begin, m1)) {
    if (m1 == end) {
      return true;
    }
    if (++m1 == end) {
      return false;
    }
    begin = m1;
    for (; m1 != end && *m1 != to_ascii('-'); ++m1) {
      // loop does the work
    }
  }

  // region
  if (isRegion(begin, m1)) {
    if (m1 == end) {
      return true;
    }
    if (++m1 == end) {
      return false;
    }
    begin = m1;
    for (; m1 != end && *m1 != to_ascii('-'); ++m1) {
      // loop does the work
    }
  }

  // variants
  while (isVariant(begin, m1)) {
    if (m1 == end) {
      return true;
    }
    if (++m1 == end) {
      return false;
    }
    begin = m1;
    for (; m1 != end && *m1 != to_ascii('-'); ++m1) {
      // loop does the work
    }
  }

  // extensions
  m2 = begin;
  ++m2;
  if (m1 == m2) {
    while (to_lower(*begin) != to_ascii('x')) {
      for (++m1; m1 != end && *m1 != to_ascii('-'); ++m1) {
        // loop does the work
      }
      if (!isExtension(begin, m1)) {
        return false;
      }
      m2 = m1;
      do {
        if (m2 == end) {
          return true;
        }
        m1 = m2;
        for (++m2; m2 != end && *m2 != to_ascii('-'); ++m2) {
          // loop does the work
        }
      } while (isExtension(begin, m2));
      if (++m1 == end) {
        return false;
      }
      begin = m1;
      if (++m1 == end || *m1 != to_ascii('-')) {
        return false;
      }
    }
  }

  // privateuse
  return isPrivateUse(begin, end);
}

template<typename iter>
bool isLanguage(iter begin, iter end) {
  iter mark;
  size_t i = 0;
  for (mark = begin; mark != end && *mark != to_ascii('-'); ++mark) {
    if (++i > 8) {
      return false;
    }
    if (!is_alpha(*mark)) {
      return false;
    }
  }
  if (i < 2) {
    return false;
  }
  return mark == end || (i <= 3 && isExtLang(++mark, end));
}

template<typename iter>
bool isExtLang(iter begin, iter end) {
  size_t n;
  for (n = 0; n < 3 && begin != end; ++n) {
    if (!(is_alpha(*begin)
          && ++begin != end && is_alpha(*begin)
          && ++begin != end && is_alpha(*begin)
          && (++begin == end
              || (*begin == to_ascii('-') && ++begin != end)))) {
      return false;
    }
  }
  return 1 <= n && n <= 3;
}

template<typename iter>
bool isScript(iter begin, iter end) {
  return begin != end && is_alpha(*begin)
      && ++begin != end && is_alpha(*begin)
      && ++begin != end && is_alpha(*begin)
      && ++begin != end && is_alpha(*begin)
      && ++begin == end;
}

template<typename iter>
bool isRegion(iter begin, iter end) {
  if (begin == end) {
    return false;
  }
  if (is_alpha(*begin)) {
    return ++begin != end && is_alpha(*begin)
        && ++begin == end;
  }
  return is_digit(*begin)
      && ++begin != end && is_digit(*begin)
      && ++begin != end && is_digit(*begin)
      && ++begin == end;
}

template<typename iter>
bool isVariant(iter begin, iter end) {
  if (begin == end) {
    return false;
  }
  iter mark = begin;
  size_t i;
  for (i = 0; i < 8 && mark != end; ++i) {
    if (!is_alnum(*mark)) {
      return false;
    }
    ++mark;
  }
  return (i == 4 && is_digit(*begin))
      || (i >= 5 && mark == end);
}

template<typename iter>
bool isExtension(iter begin, iter end) {
  if (!(begin != end && LANG_CHAR_IS_SINGLETON(*begin)
        && ++begin != end && *begin == to_ascii('-')
        && ++begin != end)) {
    return false;
  }
  while (begin != end) {
    size_t i = 0;
    for (; begin != end && *begin != to_ascii('-'); ++begin) {
      if (!is_alnum(*begin)) {
        return false;
      }
      ++i;
    }
    if (i < 2 || i > 8 || (begin != end && ++begin == end)) {
      return false;
    }
  }
  return true;
}

template<typename iter>
bool isPrivateUse(iter begin, iter end) {
  if (!(begin != end && to_lower(*begin) == to_ascii('x')
        && ++begin != end && *begin == to_ascii('-')
        && ++begin != end)) {
    return false;
  }
  while (begin != end) {
    size_t i = 0;
    for (; begin != end && *begin != to_ascii('-'); ++begin) {
      if (!is_alnum(*begin)) {
        return false;
      }
      ++i;
    }
    if (i < 1 || i > 8 || (begin != end && ++begin == end)) {
      return false;
    }
  }
  return true;
}

template<typename iter>
inline
bool isGrandfathered(iter begin, iter end) {
  return isIrregular(begin, end) || isRegular(begin, end);
}

template<typename iter>
bool isIrregular(iter begin, iter end) {
  if (begin == end) {
    return false;
  }
  if (to_lower(*begin) == to_ascii('e')) {
    return ++begin != end && to_lower(*begin) == to_ascii('n')
        && ++begin != end && *begin == to_ascii('-')
        && ++begin != end && to_lower(*begin) == to_ascii('g')
        && ++begin != end && to_lower(*begin) == to_ascii('b')
        && ++begin != end && *begin == to_ascii('-')
        && ++begin != end && to_lower(*begin) == to_ascii('o')
        && ++begin != end && to_lower(*begin) == to_ascii('e')
        && ++begin != end && to_lower(*begin) == to_ascii('d')
        && ++begin == end;
  }
  if (to_lower(*begin) == to_ascii('i')) {
    if (++begin == end || *begin != to_ascii('-') || ++begin == end) {
      return false;
    }
    if (to_lower(*begin) == to_ascii('a')) {
      return ++begin != end && to_lower(*begin) == to_ascii('m')
          && ++begin != end && to_lower(*begin) == to_ascii('i')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('b')) {
      return ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('d')) {
      return ++begin != end && to_lower(*begin) == to_ascii('e')
          && ++begin != end && to_lower(*begin) == to_ascii('f')
          && ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin != end && to_lower(*begin) == to_ascii('u')
          && ++begin != end && to_lower(*begin) == to_ascii('l')
          && ++begin != end && to_lower(*begin) == to_ascii('t')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('e')) {
      return ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin != end && to_lower(*begin) == to_ascii('o')
          && ++begin != end && to_lower(*begin) == to_ascii('c')
          && ++begin != end && to_lower(*begin) == to_ascii('h')
          && ++begin != end && to_lower(*begin) == to_ascii('i')
          && ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('h')) {
      return ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin != end && to_lower(*begin) == to_ascii('k')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('k')) {
      return ++begin != end && to_lower(*begin) == to_ascii('l')
          && ++begin != end && to_lower(*begin) == to_ascii('i')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin != end && to_lower(*begin) == to_ascii('g')
          && ++begin != end && to_lower(*begin) == to_ascii('o')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('l')) {
      return ++begin != end && to_lower(*begin) == to_ascii('u')
          && ++begin != end && to_lower(*begin) == to_ascii('x')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('m')) {
      return ++begin != end && to_lower(*begin) == to_ascii('i')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin != end && to_lower(*begin) == to_ascii('g')
          && ++begin != end && to_lower(*begin) == to_ascii('o')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('n')) {
      return ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin != end && to_lower(*begin) == to_ascii('v')
          && ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin != end && to_lower(*begin) == to_ascii('j')
          && ++begin != end && to_lower(*begin) == to_ascii('o')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('p')) {
      return ++begin != end && to_lower(*begin) == to_ascii('w')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('t')) {
      if (++begin == end) {
        return false;
      }
      if (to_lower(*begin) == to_ascii('a')) {
        if (++begin == end) {
          return false;
        }
        if (to_lower(*begin) == to_ascii('o')) {
          return ++begin == end;
        }
        if (to_lower(*begin) == to_ascii('y')) {
          return ++begin == end;
        }
        return false;
      }
      if (to_lower(*begin) == to_ascii('s')) {
        return ++begin != end && to_lower(*begin) == to_ascii('u')
            && ++begin == end;
      }
      return false;
    }
  }
  if (to_lower(*begin) == to_ascii('s')) {
    if (++begin == end || to_lower(*begin) != to_ascii('g')
        || ++begin == end || to_lower(*begin) != to_ascii('n')
        || ++begin == end || *begin != to_ascii('-')
        || ++begin == end) {
      return false;
    }
    if (to_lower(*begin) == to_ascii('b')) {
      if (++begin == end || to_lower(*begin) != to_ascii('e')
          || ++begin == end || *begin != to_ascii('-')
          || ++begin == end) {
        return false;
      }
      if (to_lower(*begin) == to_ascii('f')) {
        return ++begin != end && to_lower(*begin) == to_ascii('r')
            && ++begin == end;
      }
      if (to_lower(*begin) == to_ascii('n')) {
        return ++begin != end && to_lower(*begin) == to_ascii('l')
            && ++begin == end;
      }
      return false;
    }
    if (to_lower(*begin) == to_ascii('c')) {
      return ++begin != end && to_lower(*begin) == to_ascii('h')
          && ++begin != end && *begin == to_ascii('-')
          && ++begin != end && to_lower(*begin) == to_ascii('d')
          && ++begin != end && to_lower(*begin) == to_ascii('e')
          && ++begin == end;
    }
    return false;
  }
  return false;
}

template<typename iter>
bool isRegular(iter begin, iter end) {
  if (begin == end) {
    return false;
  }
  if (to_lower(*begin) == to_ascii('a')) {
    return ++begin != end && to_lower(*begin) == to_ascii('r')
        && ++begin != end && to_lower(*begin) == to_ascii('t')
        && ++begin != end && *begin == to_ascii('-')
        && ++begin != end && to_lower(*begin) == to_ascii('l')
        && ++begin != end && to_lower(*begin) == to_ascii('o')
        && ++begin != end && to_lower(*begin) == to_ascii('j')
        && ++begin != end && to_lower(*begin) == to_ascii('b')
        && ++begin != end && to_lower(*begin) == to_ascii('a')
        && ++begin != end && to_lower(*begin) == to_ascii('n')
        && ++begin == end;
  }
  if (to_lower(*begin) == to_ascii('c')) {
    return ++begin != end && to_lower(*begin) == to_ascii('e')
        && ++begin != end && to_lower(*begin) == to_ascii('l')
        && ++begin != end && *begin == to_ascii('-')
        && ++begin != end && to_lower(*begin) == to_ascii('g')
        && ++begin != end && to_lower(*begin) == to_ascii('a')
        && ++begin != end && to_lower(*begin) == to_ascii('u')
        && ++begin != end && to_lower(*begin) == to_ascii('l')
        && ++begin != end && to_lower(*begin) == to_ascii('i')
        && ++begin != end && to_lower(*begin) == to_ascii('s')
        && ++begin != end && to_lower(*begin) == to_ascii('h')
        && ++begin == end;
  }
  if (to_lower(*begin) == to_ascii('n')) {
    if (++begin == end || to_lower(*begin) != to_ascii('o')
        || ++begin == end || *begin != to_ascii('-')
        || ++begin == end) {
      return false;
    }
    if (to_lower(*begin) == to_ascii('b')) {
      return ++begin != end && to_lower(*begin) == to_ascii('o')
          && ++begin != end && to_lower(*begin) == to_ascii('k')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('n')) {
      return ++begin != end && to_lower(*begin) == to_ascii('y')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin == end;
    }
    return false;
  }
  if (to_lower(*begin) == to_ascii('z')) {
    if (++begin == end || to_lower(*begin) != to_ascii('h')
        || ++begin == end || *begin != to_ascii('-')
        || ++begin == end) {
      return false;
    }
    if (to_lower(*begin) == to_ascii('g')) {
      return ++begin != end && to_lower(*begin) == to_ascii('u')
          && ++begin != end && to_lower(*begin) == to_ascii('o')
          && ++begin != end && to_lower(*begin) == to_ascii('y')
          && ++begin != end && to_lower(*begin) == to_ascii('u')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('h')) {
      return ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin != end && to_lower(*begin) == to_ascii('k')
          && ++begin != end && to_lower(*begin) == to_ascii('k')
          && ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('m')) {
      if (++begin == end || to_lower(*begin) != to_ascii('i')
          || ++begin == end || to_lower(*begin) != to_ascii('n')) {
        return false;
      }
      if (++begin == end) {
        return true;
      }
      return *begin == to_ascii('-')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin == end;
    }
    if (to_lower(*begin) == to_ascii('x')) {
      return ++begin != end && to_lower(*begin) == to_ascii('i')
          && ++begin != end && to_lower(*begin) == to_ascii('a')
          && ++begin != end && to_lower(*begin) == to_ascii('n')
          && ++begin != end && to_lower(*begin) == to_ascii('g')
          && ++begin == end;
    }
    return false;
  }
  return false;
}

inline
LangTag::LangTag(const LangTag &copy) throw()
    : ascii(copy.ascii), canonical(copy.canonical),
      extlang_form(copy.extlang_form) {
  this->ascii->hold();
}

inline
LangTag::~LangTag() throw() {
  this->ascii->drop();
}

inline
bool LangTag::cmplt0(const LangTag &tag1, const LangTag &tag2) throw() {
  return LangTag::cmp(tag1, tag2) < 0;
}

inline
bool LangTag::cmpeq0(const LangTag &tag1, const LangTag &tag2) throw() {
  return LangTag::cmp(tag1, tag2) == 0;
}

inline
DPtr<uint8_t> *LangTag::getASCIIString() const throw() {
  this->ascii->hold();
  return this->ascii;
}

inline
bool LangTag::equals(const LangTag &tag) const throw() {
  return LangTag::cmp(*this, tag) == 0;
}

inline
bool LangTag::isPrivateUse() const throw() {
  return lang::isPrivateUse(this->ascii->dptr(),
      this->ascii->dptr() + this->ascii->size());
}

inline
bool LangTag::isGrandfathered() const throw() {
  return lang::isGrandfathered(this->ascii->dptr(),
      this->ascii->dptr() + this->ascii->size());
}

inline
bool LangTag::isRegularGrandfathered() const throw() {
  return isRegular(this->ascii->dptr(),
      this->ascii->dptr() + this->ascii->size());
}

inline
bool LangTag::isIrregularGrandfathered() const throw() {
  return isIrregular(this->ascii->dptr(),
      this->ascii->dptr() + this->ascii->size());
}

inline
bool LangTag::compare_first_only(const uint8_t *a, const uint8_t *b) {
  return to_lower(*a) < to_lower(*b);
}

}
