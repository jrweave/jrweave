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

#include "ucs/UTF16Iter.h"

#include "ptr/MPtr.h"

namespace ucs {

using namespace ptr;

inline
UTF16Iter::UTF16Iter() throw(BadAllocException)
    : UCSIter(), utf16str(NULL), flip(false), marker(NULL), reset_mark(NULL),
      value(0), reset_value(0) {
  try {
    NEW(this->utf16str, MPtr<uint16_t>);
  } JUST_RETHROW(BadAllocException, "(rethrow)")
}

inline
UTF16Iter::UTF16Iter(const UTF16Iter &copy)
    : UCSIter(), utf16str(copy.utf16str), marker(copy.marker),
      flip(copy.flip), value(copy.value), reset_mark(copy.reset_mark),
      reset_value(copy.reset_value) {
  this->utf16str->hold();
  this->validate_codepoints = copy.validate_codepoints;
}

inline
UTF16Iter::~UTF16Iter() {
  this->utf16str->drop();
}

inline
UTF16Iter *UTF16Iter::begin(DPtr<uint16_t> *utf16str) {
  UTF16Iter *iter;
  NEW(iter, UTF16Iter, utf16str);
  return iter;
}

inline
UTF16Iter *UTF16Iter::end(DPtr<uint16_t> *utf16str) {
  UTF16Iter *iter;
  NEW(iter, UTF16Iter, utf16str);
  return (UTF16Iter *) iter->finish();
}

inline
UCSIter *UTF16Iter::finish() {
  this->marker = NULL;
  return this;
}

inline
uint32_t UTF16Iter::current() {
  return this->value;
}

inline
bool UTF16Iter::more() {
  return this->marker != NULL;
}

inline
void UTF16Iter::mark() {
  this->reset_mark = this->marker;
  this->reset_value = this->value;
}

inline
void UTF16Iter::reset() {
  this->marker = this->reset_mark;
  this->value = this->reset_value;
}

inline
bool UTF16Iter::operator!=(UTF16Iter &rhs) {
  return !(*this == rhs);
}

inline
uint32_t UTF16Iter::operator*() {
  return this->current();
}

inline
UTF16Iter &UTF16Iter::operator++() {
  return *((UTF16Iter*)(this->advance()));
}

inline
UTF16Iter UTF16Iter::operator++(int) {
  UTF16Iter ret = *this;
  this->advance();
  return ret;
}

}
