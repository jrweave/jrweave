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

#ifndef __PTR__MPTR_H__
#define __PTR__MPTR_H__

#include <cstring>
#include "ptr/DPtr.h"

namespace ptr {

using namespace std;

template<typename ptr_type>
class MPtr : public DPtr<ptr_type> {
protected:
  virtual void destroy() throw();
  MPtr(const MPtr<ptr_type> *mptr, size_t offset) throw();
  MPtr(const MPtr<ptr_type> *mptr, size_t offset, size_t len) throw();
public:
  MPtr() throw(BadAllocException);
  MPtr(ptr_type *p) throw(BadAllocException);
  MPtr(ptr_type *p, size_t num) throw(BadAllocException);
  MPtr(size_t num) throw(BadAllocException);
  MPtr(const MPtr<ptr_type> &mptr) throw();
  MPtr(const MPtr<ptr_type> *mptr) throw();
  virtual ~MPtr() throw();

  // Overridden Methods
  virtual DPtr<ptr_type> *sub(size_t offset) throw();
  virtual DPtr<ptr_type> *sub(size_t offset, size_t len) throw();
  virtual DPtr<ptr_type> *stand() throw(BadAllocException);
  virtual bool standable() const throw();

  // Final Methods
  size_t sizeInBytes() const throw();

  // Operators
  MPtr<ptr_type> &operator=(const MPtr<ptr_type> &rhs) throw();
  MPtr<ptr_type> &operator=(const MPtr<ptr_type> *rhs) throw();
  MPtr<ptr_type> &operator=(ptr_type *p) throw(BadAllocException);
};

}

#include "ptr/MPtr-inl.h"

#endif /* __PTR__MPTR_H__ */
