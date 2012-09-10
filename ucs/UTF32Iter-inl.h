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

#include "ucs/UTF32Iter.h"

#include "ptr/MPtr.h"

namespace ucs {

using namespace ptr;

inline
UTF32Iter::UTF32Iter() throw(BadAllocException)
    : UCSIter(), utf32str(NULL), flip(false), marker(NULL), reset_mark(NULL),
      value(0), reset_value(0) {
  try {
    NEW(this->utf32str, MPtr<uint32_t>);
  } JUST_RETHROW(BadAllocException, "(rethrow)")
}

inline
UTF32Iter::UTF32Iter(const UTF32Iter &copy)
    : UCSIter(), utf32str(copy.utf32str), marker(copy.marker),
      flip(copy.flip), value(copy.value), reset_mark(copy.reset_mark),
      reset_value(copy.reset_value) {
  this->utf32str->hold();
  this->validate_codepoints = copy.validate_codepoints;
}

inline
UTF32Iter::~UTF32Iter() {
  this->utf32str->drop();
}

inline
UTF32Iter *UTF32Iter::begin(DPtr<uint32_t> *utf32str) {
  UTF32Iter *iter;
  NEW(iter, UTF32Iter, utf32str);
  return iter;
}

inline
UTF32Iter *UTF32Iter::end(DPtr<uint32_t> *utf32str) {
  UTF32Iter *iter;
  NEW(iter, UTF32Iter, utf32str);
  return (UTF32Iter *) iter->finish();
}

inline
UCSIter *UTF32Iter::finish() {
  this->marker = NULL;
  return this;
}

inline
uint32_t UTF32Iter::current() {
  return this->value;
}

inline
bool UTF32Iter::more() {
  return this->marker != NULL;
}

inline
void UTF32Iter::mark() {
  this->reset_mark = this->marker;
  this->reset_value = this->value;
}

inline
void UTF32Iter::reset() {
  this->marker = this->reset_mark;
  this->value = this->reset_value;
}

inline
bool UTF32Iter::operator!=(UTF32Iter &rhs) {
  return !(*this == rhs);
}

inline
uint32_t UTF32Iter::operator*() {
  return this->current();
}

inline
UTF32Iter &UTF32Iter::operator++() {
  return *((UTF32Iter*)(this->advance()));
}

inline
UTF32Iter UTF32Iter::operator++(int) {
  UTF32Iter ret = *this;
  this->advance();
  return ret;
}

}
