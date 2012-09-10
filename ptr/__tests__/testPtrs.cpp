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

#include "test/unit.h"

#include <iostream>
#include "ptr/APtr.h"
#include "ptr/DPtr.h"
#include "ptr/MPtr.h"
#include "ptr/OPtr.h"
#include "ptr/Ptr.h"

using namespace ptr;
using namespace std;

template<typename ptr_type>
bool testComprehensive(ptr_type *p, DPtr<ptr_type> *p2) {
  try {
    size_t size = p2->size();
    size_t offset = 1;
    size_t len = 1;
    size_t index = 1;
    DPtr<int> *p3 = p2->sub(offset);
    DPtr<int> *p4 = p3->sub(offset, len);
    PROG(p == p2->ptr());
    PROG(p + offset == p3->ptr());
    PROG(p + 2*offset == p4->ptr());
    PROG(p2->ptr() == p2->dptr());
    PROG(p3->ptr() == p3->dptr());
    PROG(p4->ptr() == p4->dptr());
    PROG(p2->sizeKnown());
    PROG(p3->sizeKnown());
    PROG(p4->sizeKnown());
    PROG(p2->size() == size);
    PROG(p3->size() == size - offset);
    PROG(p4->size() == len);
    PROG(**p2 == p[0]);
    PROG(**p3 == p[offset]);
    PROG(**p4 == p[2*offset]);
    PROG((*p2)[index] == p[index]);
    PROG((*p3)[index] == p[offset + index]);
    PROG((*p4)[index] == p[2*offset + index]);
    *p3 = p4;
    PROG(p3->ptr() == p4->ptr());
    PROG(p3->dptr() == p4->dptr());
    PROG(p3->sizeKnown() == p4->sizeKnown());
    PROG(p3->size() == p4->size());
    PROG(**p3 == **p4);
    PROG((*p3)[index] == (*p4)[index]);
    *p4 = p2;
    PROG(p2->ptr() == p4->ptr());
    PROG(p2->dptr() == p4->dptr());
    PROG(p2->sizeKnown() == p4->sizeKnown());
    PROG(p2->size() == p4->size());
    PROG(**p2 == **p4);
    PROG((*p2)[index] == (*p4)[index]);
    p4->drop();
    p3->drop();
    PASS;
  } catch (BadAllocException &e) {
    cerr << e.what();
    FAIL;
  }
}

int main(int argc, char **argv) {
  INIT;
  
  size_t size = 10;
  int *nums;
  alloc(nums, size);
  size_t i;
  for (i = 0; i < size; i++) {
    nums[i] = i;
  }

  DPtr<int> *p;
  NEW(p, DPtr<int>, nums, size);
  TEST(testComprehensive, nums, p);
  p->drop(); // won't delete nums because is DPtr

  NEW(p, MPtr<int>, nums, size);
  TEST(testComprehensive, nums, p);
  p->drop(); // will delete nums because is MPtr

  // dalloc(nums); // memory error

  NEW_ARRAY(nums, int, size);
  for (i = 0; i < size; i++) {
    nums[i] = i;
  }

  NEW(p, APtr<int>, nums, size);
  TEST(testComprehensive, nums, p);
  p->drop();

  // DELETE_ARRAY(nums); // memory error

  FINAL;
}
