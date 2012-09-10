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

#include "ptr/OPtr.h"

#include "ptr/APtr.h"
#include "ptr/MPtr.h"

namespace ptr {

using namespace std;

template<typename obj_type>
inline
OPtr<obj_type>::OPtr() throw(BadAllocException)
    : DPtr<obj_type>() {
  // do nothing
}

template<typename obj_type>
inline
OPtr<obj_type>::OPtr(obj_type *p) throw(BadAllocException)
    : DPtr<obj_type>(p, 1) {
  // do nothing
}

template<typename obj_type>
inline
OPtr<obj_type>::OPtr(const OPtr<obj_type> &optr) throw()
    : DPtr<obj_type>(&optr) {
  // do nothing
}

template<typename obj_type>
inline
OPtr<obj_type>::OPtr(const OPtr<obj_type> *optr) throw()
    : DPtr<obj_type>(optr) {
  // do nothing
}

template<typename obj_type>
inline
OPtr<obj_type>::OPtr(const OPtr<obj_type> *optr, size_t offset) throw()
    : DPtr<obj_type>(optr, offset) {
  // do nothing
}

template<typename obj_type>
inline
OPtr<obj_type>::OPtr(const OPtr<obj_type> *optr, size_t offset, size_t len)
    throw()
    : DPtr<obj_type>(optr, offset, len) {
  // do nothing
}

template<typename obj_type>
inline
OPtr<obj_type>::~OPtr() throw() {
  this->destruct();
}

template<typename obj_type>
inline
void OPtr<obj_type>::destroy() throw() {
  if (this->p != NULL) {
    DELETE((obj_type*) this->p);
  }
}

template<typename obj_type>
inline
DPtr<obj_type> *OPtr<obj_type>::sub(size_t offset) throw() {
  DPtr<obj_type> *d;
  NEW(d, OPtr<obj_type>, this, this->offset + offset);
  return d;
}

template<typename obj_type>
inline
DPtr<obj_type> *OPtr<obj_type>::sub(size_t offset, size_t len) throw() {
  DPtr<obj_type> *d;
  NEW(d, OPtr<obj_type>, this, this->offset + offset, len);
  return d;
}

template<typename obj_type>
inline
DPtr<obj_type> *OPtr<obj_type>::stand() throw(BadAllocException) {
  if (this->alone()) {
    return this;
  }
  // Making a copy of the underlying object depends on a copy constructor
  // or assignment operator.  Since we do not wish to restrict the
  // usefulness of this class to such objects, we simply return NULL.
  // In the future, it is conceivable to subclass OPtr to something
  // that will use the copy constructor or assignment operator.
  return NULL;
}

template<typename obj_type>
inline
bool OPtr<obj_type>::standable() const throw() {
  return this->alone();
}

template<typename obj_type>
OPtr<obj_type> &OPtr<obj_type>::operator=(const OPtr<obj_type> &rhs) throw() {
  DPtr<obj_type> *l = this;
  const DPtr<obj_type> *r = &rhs;
  *l = *r;
  return *this;
}

template<typename obj_type>
OPtr<obj_type> &OPtr<obj_type>::operator=(const OPtr<obj_type> *rhs) throw() {
  DPtr<obj_type> *l = this;
  const DPtr<obj_type> *r = rhs;
  *l = r;
  return *this;
}

template<typename obj_type>
inline
OPtr<obj_type> &OPtr<obj_type>::operator=(obj_type *p)
    throw(BadAllocException) {
  DPtr<obj_type>::operator=(p);
  return *this;
}

}
