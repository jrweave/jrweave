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
#include "ucs/InvalidCodepointException.h"
#include "ucs/nf.h"
#include "ucs/utf.h"

namespace ucs {

using namespace ptr;

UTF32Iter::UTF32Iter(DPtr<uint32_t> *utf32str) throw(SizeUnknownException)
    : UCSIter(), utf32str(utf32str), flip(false), marker(NULL),
      reset_mark(NULL), value(0), reset_value(0) {
  if (!this->utf32str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  this->utf32str->hold();
  if (this->utf32str->size() > 0) {
    this->flip = utf32flip(this->utf32str, &(this->marker));
    if (this->utf32str->dptr() + this->utf32str->size() == this->marker) {
      this->marker = NULL;
      this->value = 0;
    } else {
      this->value = utf32char(this->marker, this->flip, &(this->marker));
    }
    this->reset_mark = this->marker;
    this->reset_value = this->value;
  }
}

UCSIter *UTF32Iter::start() {
  if (this->utf32str->size() > 0) {
    this->flip = utf32flip(this->utf32str, &(this->marker));
    this->value = utf32char(this->marker, this->flip, &(this->marker));
    if (this->marker == this->utf32str->dptr() + this->utf32str->size()) {
      this->marker = NULL;
    }
    if (this->validate_codepoints && !nfvalid(this->value)) {
      THROW(InvalidCodepointException, this->value);
    }
  }
  return this;
}

UCSIter *UTF32Iter::advance() {
  if (this->marker == NULL) {
    return NULL;
  }
  if (this->marker == this->utf32str->dptr() + this->utf32str->size()) {
    this->marker = NULL;
  } else {
    this->value = utf32char(this->marker, this->flip, &(this->marker));
    if (this->validate_codepoints && !nfvalid(this->value)) {
      THROW(InvalidCodepointException, this->value);
    }
  }
  return this;
}

UTF32Iter &UTF32Iter::operator=(UTF32Iter &rhs) {
  if (this != &rhs) {
    this->utf32str->drop();
    this->utf32str = rhs.utf32str;
    this->utf32str->hold();
    this->flip = rhs.flip;
    this->marker = rhs.marker;
    this->value = rhs.value;
    this->reset_mark = rhs.reset_mark;
    this->reset_value = rhs.reset_value;
    this->validate_codepoints = rhs.validate_codepoints;
  }
  return *this;
}

bool UTF32Iter::operator==(UTF32Iter &rhs) {
  return this->marker == rhs.marker &&
      this->utf32str->dptr() == rhs.utf32str->dptr() &&
      this->utf32str->size() == rhs.utf32str->size() &&
      (this->marker == NULL || this->value == rhs.value) &&
      (this->flip == rhs.flip);
}

}
