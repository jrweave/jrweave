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

#ifndef __PTR__OPTR_H__
#define __PTR__OPTR_H__

#include "ptr/DPtr.h"

namespace ptr {

using namespace std;

template<typename obj_type>
class OPtr : public DPtr<obj_type> {
protected:
  virtual void destroy() throw();
  OPtr(const OPtr<obj_type> *optr, size_t offset) throw();
  OPtr(const OPtr<obj_type> *optr, size_t offset, size_t len) throw();
public:
  OPtr() throw(BadAllocException);
  OPtr(obj_type *p) throw(BadAllocException);
  OPtr(const OPtr<obj_type> &optr) throw();
  OPtr(const OPtr<obj_type> *optr) throw();
  virtual ~OPtr() throw();

  // Overridden Methods
  virtual DPtr<obj_type> *sub(size_t offset) throw();
  virtual DPtr<obj_type> *sub(size_t offset, size_t len) throw();
  virtual DPtr<obj_type> *stand() throw(BadAllocException);
  virtual bool standable() const throw();

  // Operators
  OPtr<obj_type> &operator=(const OPtr<obj_type> &rhs) throw();
  OPtr<obj_type> &operator=(const OPtr<obj_type> *rhs) throw();
  OPtr<obj_type> &operator=(obj_type *p) throw(BadAllocException);
};

}

#include "ptr/OPtr-inl.h"

#endif /* __PTR__OPTR_H__ */
