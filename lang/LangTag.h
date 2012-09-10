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

#ifndef __LANG__LANGTAG_H__
#define __LANG__LANGTAG_H__

#include <iostream>
#include "lang/MalformedLangTagException.h"
#include "ptr/DPtr.h"
#include "sys/char.h"
#include "sys/ints.h"

#define LANG_CHAR_IS_SINGLETON(c) (is_digit(c) || (is_alpha(c) && c != to_ascii('x')))

namespace lang {

using namespace ex;
using namespace ptr;
using namespace std;

#include "lang/lang_arrays.h"

template<typename iter>
bool isLanguageTag(iter begin, iter end);

template<typename iter>
bool isLangTag(iter begin, iter end);

template<typename iter>
bool isPrivateUse(iter begin, iter end);

template<typename iter>
bool isGrandfathered(iter begin, iter end);

template<typename iter>
bool isLanguage(iter begin, iter end);

template<typename iter>
bool isScript(iter begin, iter end);

template<typename iter>
bool isRegion(iter begin, iter end);

template<typename iter>
bool isVariant(iter begin, iter end);

template<typename iter>
bool isExtension(iter begin, iter end);

template<typename iter>
bool isExtLang(iter begin, iter end);

template<typename iter>
bool isIrregular(iter begin, iter end);

template<typename iter>
bool isRegular(iter begin, iter end);

enum LangTagPart {
  LANGUAGE, SCRIPT, REGION, VARIANTS, EXTENSIONS, PRIVATE_USE
};

class LangTag {
protected:
  DPtr<uint8_t> *ascii;
  bool canonical;
  bool extlang_form;
  static const uint8_t *lookup(const uint8_t *key,
                               size_t len,
                               const void **values,
                               size_t &outlen) throw();
  static bool compare_first_only(const uint8_t *a, const uint8_t *b);
  bool replaceSection(DPtr<uint8_t> *part, const void **lookup_array)
      throw(BadAllocException);
  void normalizePart(enum LangTagPart p)
      throw(BadAllocException);
public:
  LangTag() throw(BadAllocException); // "i-default"
  LangTag(DPtr<uint8_t> *ascii)
      throw(SizeUnknownException, MalformedLangTagException);
  LangTag(const LangTag &copy) throw();
  virtual ~LangTag() throw();

  // Static Methods
  static int cmp(const LangTag &tag1, const LangTag &tag2) throw();
  static bool cmplt0(const LangTag &tag1, const LangTag &tag2) throw();
  static bool cmpeq0(const LangTag &tag1, const LangTag &tag2) throw();

  // Final Methods
  DPtr<uint8_t> *getASCIIString() const throw();
  bool equals(const LangTag &tag) const throw();
  DPtr<uint8_t> *getPart(const enum LangTagPart part) const throw();
  bool isPrivateUse() const throw();
  bool isGrandfathered() const throw();
  bool isRegularGrandfathered() const throw();
  bool isIrregularGrandfathered() const throw();

  // Virtual Methods
  virtual LangTag *normalize() throw(BadAllocException);
  virtual LangTag *extlangify() throw(BadAllocException);

  // Operators
  LangTag &operator=(const LangTag &rhs) throw();
};

} // end namespace lang

std::istream& operator>>(std::istream& stream, lang::LangTag &tag);
std::ostream& operator<<(std::ostream& stream, const lang::LangTag &tag);

#include "lang/LangTag-inl.h"

#endif /* __LANG__LANGTAG_H__ */
