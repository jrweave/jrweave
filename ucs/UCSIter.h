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

#ifndef __UCS__UCSITER_H__
#define __UCS__UCSITER_H__

#include "sys/ints.h"
#include "ucs/utf.h"

namespace ucs {

using namespace std;

class UCSIter {
protected:
  UCSIter();
  bool validate_codepoints;
public:
  virtual ~UCSIter();
  virtual void validate(const bool v);
  virtual UCSIter *start() = 0;
  virtual UCSIter *finish() = 0;
  virtual uint32_t current() = 0;
  virtual size_t current(uint8_t *utf8);
  virtual size_t current(uint16_t *utf16, const enum BOM bom);
  virtual size_t current(uint32_t *utf32, const enum BOM bom);
  virtual UCSIter *advance() = 0;
  virtual bool more() = 0;
  virtual uint32_t next();
  virtual size_t next(uint8_t *utf8);
  virtual size_t next(uint16_t *utf16, const enum BOM bom);
  virtual size_t next(uint32_t *utf32, const enum BOM bom);
  virtual void mark() = 0;
  virtual void reset() = 0;
};

}

#include "ucs/UCSIter-inl.h"

#endif /* __UCS_UCSITER_H__ */
