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
#include "ucs/InvalidCodepointException.h"
#include "ucs/nf.h"
#include "ucs/utf.h"

namespace ucs {

using namespace ptr;

UTF16Iter::UTF16Iter(DPtr<uint16_t> *utf16str) throw(SizeUnknownException)
    : UCSIter(), utf16str(utf16str), flip(false), marker(NULL),
      reset_mark(NULL), value(0), reset_value(0) {
  if (!this->utf16str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  this->utf16str->hold();
  if (this->utf16str->size() > 0) {
    this->flip = utf16flip(this->utf16str, &(this->marker));
    if (this->utf16str->dptr() + this->utf16str->size() == this->marker) {
      this->marker = NULL;
      this->value = 0;
    } else {
      this->value = utf16char(this->marker, this->flip, &(this->marker));
    }
    this->reset_mark = this->marker;
    this->reset_value = this->value;
  }
}

UCSIter *UTF16Iter::start() {
  if (this->utf16str->size() > 0) {
    this->flip = utf16flip(this->utf16str, &(this->marker));
    this->value = utf16char(this->marker, this->flip, &(this->marker));
    if (this->marker == this->utf16str->dptr() + this->utf16str->size()) {
      this->marker = NULL;
    }
    if (this->validate_codepoints && !nfvalid(this->value)) {
      THROW(InvalidCodepointException, this->value);
    }
  }
  return this;
}

UCSIter *UTF16Iter::advance() {
  if (this->marker == NULL) {
    return NULL;
  }
  if (this->marker == this->utf16str->dptr() + this->utf16str->size()) {
    this->marker = NULL;
  } else {
    this->value = utf16char(this->marker, this->flip, &(this->marker));
    if (this->validate_codepoints && !nfvalid(this->value)) {
      THROW(InvalidCodepointException, this->value);
    }
  }
  return this;
}

UTF16Iter &UTF16Iter::operator=(UTF16Iter &rhs) {
  if (this != &rhs) {
    this->utf16str->drop();
    this->utf16str = rhs.utf16str;
    this->utf16str->hold();
    this->flip = rhs.flip;
    this->marker = rhs.marker;
    this->value = rhs.value;
    this->reset_mark = rhs.reset_mark;
    this->reset_value = rhs.reset_value;
    this->validate_codepoints = rhs.validate_codepoints;
  }
  return *this;
}

bool UTF16Iter::operator==(UTF16Iter &rhs) {
  return this->marker == rhs.marker &&
      this->utf16str->dptr() == rhs.utf16str->dptr() &&
      this->utf16str->size() == rhs.utf16str->size() &&
      (this->marker == NULL || this->value == rhs.value) &&
      (this->flip == rhs.flip);
}

}
