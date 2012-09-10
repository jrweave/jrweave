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

#include "ucs/UTF8Iter.h"

#include "ptr/MPtr.h"

namespace ucs {

using namespace ptr;

inline
UTF8Iter::UTF8Iter() throw(BadAllocException)
    : UCSIter(), utf8str(NULL), marker(NULL), reset_mark(NULL), value(0),
      reset_value(0) {
  try {
    NEW(this->utf8str, MPtr<uint8_t>);
  } JUST_RETHROW(BadAllocException, "(rethrow)")
}

inline
UTF8Iter::UTF8Iter(const UTF8Iter &copy)
    : UCSIter(), utf8str(copy.utf8str), marker(copy.marker),
      value(copy.value), reset_mark(copy.reset_mark),
      reset_value(copy.reset_value) {
  this->utf8str->hold();
  this->validate_codepoints = copy.validate_codepoints;
}

inline
UTF8Iter::~UTF8Iter() {
  this->utf8str->drop();
}

inline
UTF8Iter *UTF8Iter::begin(DPtr<uint8_t> *utf8str) {
  UTF8Iter *iter;
  NEW(iter, UTF8Iter, utf8str);
  return iter;
}

inline
UTF8Iter *UTF8Iter::end(DPtr<uint8_t> *utf8str) {
  UTF8Iter *iter;
  NEW(iter, UTF8Iter, utf8str);
  return (UTF8Iter *)iter->finish();
}

inline
UCSIter *UTF8Iter::finish() {
  this->marker = NULL;
  return this;
}

inline
uint32_t UTF8Iter::current() {
  return this->value;
}

inline
bool UTF8Iter::more() {
  return this->marker != NULL;
}

inline
void UTF8Iter::mark() {
  this->reset_mark = this->marker;
  this->reset_value = this->value;
}

inline
void UTF8Iter::reset() {
  this->marker = this->reset_mark;
  this->value = this->reset_value;
}

inline
bool UTF8Iter::operator==(UTF8Iter &rhs) {
  return this->marker == rhs.marker &&
      this->utf8str->dptr() == rhs.utf8str->dptr() &&
      this->utf8str->size() == rhs.utf8str->size() &&
      (this->marker == NULL || this->value == rhs.value);
}

inline
bool UTF8Iter::operator!=(UTF8Iter &rhs) {
  return !(*this == rhs);
}

inline
uint32_t UTF8Iter::operator*() {
  return this->current();
}

inline
UTF8Iter &UTF8Iter::operator++() {
  return *((UTF8Iter*)(this->advance()));
}

inline
UTF8Iter UTF8Iter::operator++(int) {
  UTF8Iter ret = *this;
  this->advance();
  return ret;
}

}
