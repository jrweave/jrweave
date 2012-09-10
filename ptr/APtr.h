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

#ifndef __PTR__APTR_H__
#define __PTR__APTR_H__

#include <cstring>
#include "ptr/DPtr.h"

namespace ptr {

using namespace std;

template<typename arr_type>
class APtr : public DPtr<arr_type> {
protected:
  size_t actual_num;
  virtual void destroy() throw();
  APtr(const APtr<arr_type> *aptr, size_t offset) throw();
  APtr(const APtr<arr_type> *aptr, size_t offset, size_t len) throw();
public:
  APtr() throw(BadAllocException);
  APtr(arr_type *p) throw(BadAllocException);
  APtr(arr_type *p, size_t num) throw();
  APtr(size_t num) throw(BadAllocException);
  APtr(const APtr<arr_type> &mptr) throw();
  APtr(const APtr<arr_type> *mptr) throw();
  virtual ~APtr() throw();

  // Overridden Methods
  virtual DPtr<arr_type> *sub(size_t offset) throw();
  virtual DPtr<arr_type> *sub(size_t offset, size_t len) throw();
  virtual DPtr<arr_type> *stand() throw(BadAllocException);
  virtual bool standable() const throw();

  // Operators
  APtr<arr_type> &operator=(const APtr<arr_type> &rhs) throw();
  APtr<arr_type> &operator=(const APtr<arr_type> *rhs) throw();
  APtr<arr_type> &operator=(arr_type *p) throw(BadAllocException);
};

}

#include "ptr/APtr-inl.h"

#endif /* __PTR__APTR_H__ */
