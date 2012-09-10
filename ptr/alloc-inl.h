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

#include "ptr/alloc.h"

#include <cstdlib>
#ifdef PTR_MEMDEBUG
#include <iostream>
#include <set>
#endif

namespace ptr {

using namespace std;

template<typename ptr_type>
bool alloc(ptr_type *&p, size_t num) throw() {
  #ifdef PTR_MEMDEBUG
    if (num <= 0) {
        cerr << "[PTR_MEMDEBUG] Warning: requested an allocation of zero bytes." << endl;
    }
  #endif
  p = (ptr_type*)calloc(num, sizeof(ptr_type));
  if (p != NULL) {
    PTR_PRINTA(p);
    #ifdef PTR_MEMDEBUG
      if (!ptr::__persist_ptrs && !ptr::__PTRS.insert((void*)p).second) {
        cerr << "[PTR_MEMDEBUG] Unexpected allocation to " << p << ", which means whatever was previously allocated to that address was not deallocated using alloc.h.\n\talloc(" << p << ", " << num << "[ * " << sizeof(ptr_type) << "]);" << endl;
      }
    #endif
    return true;
  }
  return false;
}

template<typename ptr_type>
bool ralloc(ptr_type *&p, size_t num) throw() {
  if (p == NULL) {
    return alloc(p, num);
  }
  ptr_type *q = (ptr_type *)realloc(p, num*sizeof(ptr_type));
  if (q == NULL) {
    return false;
  }
  PTR_PRINTD(p);
  PTR_PRINTA(q);
  #ifdef PTR_MEMDEBUG
    if (ptr::__PTRS.erase((void*)p) != 1 && !ptr::__persist_ptrs) {
      cerr << "[PTR_MEMDEBUG] Reallocated away from " << p << ", but there is no record of allocation at that address.\n\tralloc(" << p << ", " << num << "[ * " << sizeof(ptr_type) << "]);" << endl;
    }
    if (!ptr::__persist_ptrs && !ptr::__PTRS.insert((void*)q).second) {
      cerr << "[PTR_MEMDEBUG] Unexpected allocation to " << q << ", which means whatever was previously allocated to that address was not deallocated using alloc.h.\n\tralloc(" << p << ", " << num << "[ * " << sizeof(ptr_type) << "]);" << endl;
    }
  #endif
  p = q;
  return true;
}

template<typename ptr_type>
void dalloc(ptr_type *&p) throw() {
  if (p != NULL) {
    PTR_PRINTD(p);
    #ifdef PTR_MEMDEBUG
      if (ptr::__PTRS.erase((void*)p) != 1 && !ptr::__persist_ptrs) {
        cerr << "[PTR_MEMDEBUG] Deallocated " << p << ", but there is no record of allocation at that address.\n\tdalloc(" << p << ");" << endl;
      }
    #endif
    free(p);
    p = NULL;
  }
  #ifdef PTR_MEMDEBUG
    else {
      cerr << "[PTR_MEMDEBUG] Warning: attempt to deallocate NULL pointer." << endl;
    }
  #endif
}

}
