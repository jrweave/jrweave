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

#ifndef __LANG__LANGRANGE_H__
#define __LANG__LANGRANGE_H__

#include <iostream>
#include "lang/LangTag.h"
#include "lang/MalformedLangRangeException.h"
#include "ptr/BadAllocException.h"
#include "ptr/DPtr.h"
#include "sys/ints.h"

#define LANG_CHAR_ASTERISK UINT32_C(0x2A)

namespace lang {

using namespace ex;
using namespace ptr;
using namespace std;

template<typename iter>
bool isLanguageRange(iter begin, iter end);

template<typename iter>
bool isExtendedLanguageRange(iter begin, iter end);

class LangRange {
protected:
  DPtr<uint8_t> *ascii;
public:
  LangRange() throw(BadAllocException); // "*"
  LangRange(DPtr<uint8_t> *ascii) throw(MalformedLangRangeException);
  LangRange(const LangRange &copy) throw();
  virtual ~LangRange() throw();

  // Static Methods
  static int cmp(const LangRange &rng1, const LangRange &rng2) throw();
  static bool cmplt0(const LangRange &rng1, const LangRange &rng2) throw();
  static bool cmpeq0(const LangRange &rng1, const LangRange &rng2) throw();

  // Final Methods
  DPtr<uint8_t> *getASCIIString() const throw();
  bool equals(const LangRange &rng) const throw();
  bool isBasic() const throw();
  bool matches(LangTag *lang_tag) const throw();
  bool matches(LangTag *lang_tag, bool basic) const throw();

  // Operators
  LangRange &operator=(const LangRange &rhs) throw();
};

} // end namespace lang

std::istream& operator>>(std::istream& stream, lang::LangRange &rng);
std::ostream& operator<<(std::ostream& stream, const lang::LangRange &rng);

#include "lang/LangRange-inl.h"

#endif /* __LANG__LANGRANGE_H__ */
