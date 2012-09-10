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

#include "ptr/DPtr.h"

#include "ptr/alloc.h"

namespace ptr {

using namespace std;

template<typename ptr_type>
inline
DPtr<ptr_type>::DPtr() throw(BadAllocException)
    : Ptr(), num(0), offset(0), size_known(true) {
  // do nothing
}

template<typename ptr_type>
inline
DPtr<ptr_type>::DPtr(ptr_type *p) throw(BadAllocException)
    : Ptr((void*)p), num(0), offset(0), size_known(false) {
  // do nothing
}

template<typename ptr_type>
inline
DPtr<ptr_type>::DPtr(ptr_type *p, size_t size) throw(BadAllocException)
    : Ptr((void*)p), num(size), offset(0), size_known(true) {
  // do nothing
}

template<typename ptr_type>
inline
DPtr<ptr_type>::DPtr(const DPtr<ptr_type> &dptr) throw()
    : Ptr(&dptr), num(dptr.size()), offset(0), size_known(dptr.sizeKnown()) {
  // do nothing
}

template<typename ptr_type>
inline
DPtr<ptr_type>::DPtr(const DPtr<ptr_type> *dptr) throw()
    : Ptr(dptr), num(dptr->size()), offset(0), size_known(dptr->sizeKnown()) {
  // do nothing
}

template<typename ptr_type>
inline
DPtr<ptr_type>::DPtr(const DPtr<ptr_type> *dptr, size_t offset)
    throw()
    : Ptr(dptr), num(dptr->sizeKnown() ? dptr->size() - offset : 0),
      offset(offset), size_known(dptr->sizeKnown()) {
  // do nothing
}

template<typename ptr_type>
inline
DPtr<ptr_type>::DPtr(const DPtr<ptr_type> *dptr, size_t offset, size_t len)
    throw()
    : Ptr(dptr), num(len), offset(offset), size_known(true) {
  // do nothing
}

template<typename ptr_type>
inline
DPtr<ptr_type>::~DPtr() throw() {
  this->destruct();
}

template<typename ptr_type>
void DPtr<ptr_type>::reset(ptr_type *p, bool sizeknown, size_t size)
    THROWS(BadAllocException) {
  Ptr::reset(p);
  this->size_known = sizeknown;
  this->num = sizeknown ? size : 0;
  this->offset = 0;
}
TRACE(BadAllocException, "(trace)")

template<typename ptr_type>
void *DPtr<ptr_type>::ptr() const throw() {
  return (void *)this->dptr();
}

template<typename ptr_type>
inline
ptr_type *DPtr<ptr_type>::dptr() const throw() {
  return ((ptr_type *)this->p) + offset;
}

template<typename ptr_type>
inline
bool DPtr<ptr_type>::sizeKnown() const throw() {
  return this->size_known;
}

template<typename ptr_type>
inline
size_t DPtr<ptr_type>::size() const throw() {
  return this->num;
}

template<typename ptr_type>
inline
DPtr<ptr_type> *DPtr<ptr_type>::sub(size_t offset) throw() {
  DPtr<ptr_type> *d;
  NEW(d, DPtr<ptr_type>, this, this->offset + offset);
  return d;
}

template<typename ptr_type>
inline
DPtr<ptr_type> *DPtr<ptr_type>::sub(size_t offset, size_t len) throw() {
  DPtr<ptr_type> *d;
  NEW(d, DPtr<ptr_type>, this, this->offset + offset, len);
  return d;
}

template<typename ptr_type>
inline
DPtr<ptr_type> *DPtr<ptr_type>::stand() throw(BadAllocException) {
  return this->alone() ? this : NULL;
}

template<typename ptr_type>
inline
bool DPtr<ptr_type>::standable() const throw() {
  return this->alone();
}

template<typename ptr_type>
DPtr<ptr_type> &DPtr<ptr_type>::operator=(const DPtr<ptr_type> &rhs) throw() {
  Ptr *l = this;
  const Ptr *r = &rhs;
  *l = *r;
  this->num = rhs.num;
  this->offset = rhs.offset;
  this->size_known = rhs.size_known;
  return *this;
}

template<typename ptr_type>
DPtr<ptr_type> &DPtr<ptr_type>::operator=(const DPtr<ptr_type> *rhs) throw() {
  Ptr *l = this;
  const Ptr *r = rhs;
  *l = r;
  this->num = rhs->num;
  this->offset = rhs->offset;
  this->size_known = rhs->size_known;
  return *this;
}

template<typename ptr_type>
DPtr<ptr_type> &DPtr<ptr_type>::operator=(ptr_type *p)
    throw(BadAllocException) {
  Ptr::operator=((void*)p);
  this->num = 0;
  this->offset = 0;
  this->size_known = false;
  return *this;
}

template<typename ptr_type>
inline
ptr_type &DPtr<ptr_type>::operator*() const throw() {
  return *(this->dptr());
}

template<typename ptr_type>
inline
ptr_type &DPtr<ptr_type>::operator[](const size_t i) const throw() {
  return (this->dptr())[i];
}

template<typename ptr_type>
inline
ptr_type *DPtr<ptr_type>::operator->() const throw() {
  return this->dptr();
}

}
