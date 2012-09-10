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

#include "ptr/APtr.h"

#include <algorithm>
#include <new>
#include "ptr/alloc.h"

namespace ptr {

using namespace std;

template<typename arr_type>
inline
APtr<arr_type>::APtr() throw(BadAllocException)
    : DPtr<arr_type>(), actual_num(0) {
  // do nothing
}

template<typename arr_type>
inline
APtr<arr_type>::APtr(arr_type *p) throw(BadAllocException)
    : DPtr<arr_type>(p, 0), actual_num(0) {
  this->size_known = false;
}

template<typename arr_type>
inline
APtr<arr_type>::APtr(arr_type *p, size_t num) throw()
    : DPtr<arr_type>(p, num), actual_num(num) {
  // do nothing
}

template<typename arr_type>
APtr<arr_type>::APtr(size_t num) throw(BadAllocException)
    : DPtr<arr_type>((arr_type*)NULL, num), actual_num(num) {
  try {
    NEW_ARRAY(this->p, arr_type, num);
  } catch (bad_alloc &ba) {
    // num*sizeof(arr_type) may be a low estimate since
    // we're creating an array with new and not malloc
    THROW(BadAllocException, num*sizeof(arr_type));
  }
}

template<typename arr_type>
inline
APtr<arr_type>::APtr(const APtr<arr_type> &aptr) throw()
    : DPtr<arr_type>(&aptr), actual_num(aptr.actual_num) {
  // do nothing
}

template<typename arr_type>
inline
APtr<arr_type>::APtr(const APtr<arr_type> *aptr) throw()
    : DPtr<arr_type>(aptr), actual_num(aptr->actual_num) {
  // do nothing
}

template<typename arr_type>
inline
APtr<arr_type>::APtr(const APtr<arr_type> *aptr, size_t offset) throw()
    : DPtr<arr_type>(aptr, offset), actual_num(aptr->actual_num) {
  // do nothing
}

template<typename arr_type>
inline
APtr<arr_type>::APtr(const APtr<arr_type> *aptr, size_t offset, size_t len)
    throw()
    : DPtr<arr_type>(aptr, offset, len), actual_num(aptr->actual_num) {
  // do nothing
}

template<typename arr_type>
inline
APtr<arr_type>::~APtr() throw() {
  this->destruct();
}

template<typename arr_type>
inline
void APtr<arr_type>::destroy() throw() {
  if (this->p != NULL) {
    DELETE_ARRAY((arr_type *)this->p);
  }
}

template<typename arr_type>
inline
DPtr<arr_type> *APtr<arr_type>::sub(size_t offset) throw() {
  DPtr<arr_type> *d;
  NEW(d, APtr<arr_type>, this, this->offset + offset);
  return d;
}

template<typename arr_type>
inline
DPtr<arr_type> *APtr<arr_type>::sub(size_t offset, size_t len) throw() {
  DPtr<arr_type> *d;
  NEW(d, APtr<arr_type>, this, this->offset + offset, len);
  return d;
}

template<typename arr_type>
DPtr<arr_type> *APtr<arr_type>::stand() throw(BadAllocException) {
  if (!this->sizeKnown()) {
    return NULL;
  }
  if (this->alone() && this->offset == 0 && this->size() == this->actual_num) {
    return this;
  }
  arr_type *arr = NULL;
  try {
    NEW_ARRAY(arr, arr_type, this->size());
  } RETHROW_BAD_ALLOC
  copy(this->dptr(), this->dptr() + this->size(), arr);
  if (this->localRefs() > 1) {
    DPtr<arr_type> *d;
    try {
      NEW(d, APtr<arr_type>, arr, this->size());
    } catch (BadAllocException &e) {
      DELETE_ARRAY(arr);
      RETHROW(e, "(rethrow)");
    }
    this->actual_num = this->size();
    this->drop();
    return d;
  }
  this->actual_num = this->size();
  DPtr<arr_type>::reset(arr, true, this->size());
  return this;
}

template<typename arr_type>
inline
bool APtr<arr_type>::standable() const throw() {
  return this->alone() || this->sizeKnown();
}

template<typename arr_type>
APtr<arr_type> &APtr<arr_type>::operator=(const APtr<arr_type> &rhs) throw() {
  DPtr<arr_type> *l = this;
  const DPtr<arr_type> *r = &rhs;
  *l = *r;
  this->actual_num = rhs.actual_num;
  return *this;
}

template<typename arr_type>
APtr<arr_type> &APtr<arr_type>::operator=(const APtr<arr_type> *rhs) throw() {
  DPtr<arr_type> *l = this;
  const DPtr<arr_type> *r = rhs;
  *l = r;
  this->actual_num = rhs->actual_num;
  return *this;
}

template<typename arr_type>
inline
APtr<arr_type> &APtr<arr_type>::operator=(arr_type *p)
    throw(BadAllocException) {
  DPtr<arr_type>::operator=(p);
  this->actual_num = 0;
  return *this;
}

}
