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

#ifndef __UCS__UTF8ITER_H__
#define __UCS__UTF8ITER_H__

#include "ptr/DPtr.h"
#include "ptr/SizeUnknownException.h"
#include "sys/ints.h"
#include "ucs/UCSIter.h"

namespace ucs {

using namespace ptr;
using namespace std;

class UTF8Iter : public UCSIter {
private:
  DPtr<uint8_t> *utf8str;
  const uint8_t *reset_mark;
  const uint8_t *marker;
  uint32_t reset_value;
  uint32_t value;
public:
  typedef input_iterator_tag iterator_category;
  typedef uint32_t value_type;
  typedef size_t difference_type;
  typedef uint32_t* pointer;
  typedef uint32_t& reference;

  UTF8Iter() throw(BadAllocException);
  UTF8Iter(DPtr<uint8_t> *utf8str) throw(SizeUnknownException);
  UTF8Iter(const UTF8Iter &copy);
  virtual ~UTF8Iter();

  // Static Methods
  static UTF8Iter *begin(DPtr<uint8_t> *utf8str);
  static UTF8Iter *end(DPtr<uint8_t> *utf8str);

  // Implemented Abstract Methods
  UCSIter *start();
  UCSIter *finish();
  uint32_t current();
  UCSIter *advance();
  bool more();
  void mark();
  void reset();

  // Operators
  UTF8Iter &operator=(UTF8Iter &rhs);
  bool operator==(UTF8Iter &rhs);
  bool operator!=(UTF8Iter &rhs);
  uint32_t operator*();
  UTF8Iter &operator++();
  UTF8Iter operator++(int);
};

}

#include "ucs/UTF8Iter-inl.h"

#endif /* __UCS__UTF8ITER_H__ */
