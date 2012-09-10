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

#include "ucs/UCSIter.h"

#include "sys/ints.h"
#include "ucs/utf.h"

namespace ucs {

using namespace std;

inline
UCSIter::UCSIter()
    : validate_codepoints(false) {
  // do nothing
}

inline
UCSIter::~UCSIter() {
  // do nothing
}

inline
void UCSIter::validate(const bool v) {
  this->validate_codepoints = v;
}

inline
size_t UCSIter::current(uint8_t *utf8) {
  return utf8len(this->current(), utf8);
}

inline
size_t UCSIter::current(uint16_t *utf16, const enum BOM bom) {
  return utf16len(this->current(), bom, utf16);
}

inline
size_t UCSIter::current(uint32_t *utf32, const enum BOM bom) {
  return utf32len(this->current(), bom, utf32);
}

inline
uint32_t UCSIter::next() {
  uint32_t codepoint = this->current();
  this->advance();
  return codepoint;
}

inline
size_t UCSIter::next(uint8_t *utf8) {
  size_t len = this->current(utf8);
  this->advance();
  return len;
}

inline
size_t UCSIter::next(uint16_t *utf16, const enum BOM bom) {
  size_t len = this->current(utf16, bom);
  this->advance();
  return len;
}

inline
size_t UCSIter::next(uint32_t *utf32, const enum BOM bom) {
  size_t len = this->current(utf32, bom);
  this->advance();
  return len;
}

}
