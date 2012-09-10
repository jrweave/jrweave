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

#ifndef __UCS__UTF16ITER_H__
#define __UCS__UTF16ITER_H__

#include "ptr/DPtr.h"
#include "ptr/SizeUnknownException.h"
#include "sys/ints.h"
#include "ucs/UCSIter.h"

namespace ucs {

using namespace ptr;
using namespace std;

class UTF16Iter : public UCSIter {
private:
  DPtr<uint16_t> *utf16str;
  const uint16_t *reset_mark;
  const uint16_t *marker;
  uint32_t reset_value;
  uint32_t value;
  bool flip;
public:
  typedef input_iterator_tag iterator_category;
  typedef uint32_t value_type;
  typedef size_t difference_type;
  typedef uint32_t* pointer;
  typedef uint32_t& reference;

  UTF16Iter() throw(BadAllocException);
  UTF16Iter(DPtr<uint16_t> *utf16str) throw(SizeUnknownException);
  UTF16Iter(const UTF16Iter &copy);
  virtual ~UTF16Iter();

  // Static Methods
  static UTF16Iter *begin(DPtr<uint16_t> *utf16str);
  static UTF16Iter *end(DPtr<uint16_t> *utf16str);

  // Implemented Abstract Methods
  UCSIter *start();
  UCSIter *finish();
  uint32_t current();
  UCSIter *advance();
  bool more();
  void mark();
  void reset();

  // Operators
  UTF16Iter &operator=(UTF16Iter &rhs);
  bool operator==(UTF16Iter &rhs);
  bool operator!=(UTF16Iter &rhs);
  uint32_t operator*();
  UTF16Iter &operator++();
  UTF16Iter operator++(int);
};

}

#include "ucs/UTF16Iter-inl.h"

#endif /* __UCS__UTF16ITER_H__ */
