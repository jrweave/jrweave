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

#include <vector>
#include "test/unit.h"

using namespace ptr;
using namespace std;

bool testVector() THROWS(BadAllocException) {
  int i;
  int nums = 11;
  vector<int > *vec;
  NEW(vec, vector<int>);
  for (i = 0; i < nums; i++) {
    vec->push_back(i);
  }
  OPtr<vector<int> > *p;
  NEW(p, OPtr<vector<int> >, vec);
  PROG((vector<int>*)p->ptr() == vec);
  PROG(p->dptr() == vec);
  PROG(**p == *vec);
  PROG((*p)->at(1) == vec->at(1));
  PROG(p->sizeKnown());
  PROG(p->size() == 1);
  int max = 10;
  for (i = 0; i < max; i++) {
    p->hold();
  }
  for (i = 0; i < max; i++) {
    p->drop();
  }
  PROG(p->dptr() == vec);
  vector<int> *vec2;
  NEW(vec2, vector<int>);
  *p = vec2;
  // vec is no longer a valid pointer
  // vec->push_back(100); // should create memory error
  PROG(p->dptr() == vec2);
  PROG((**p).size() == 0);
  p->hold();
  p->drop();
  p->drop(); // this deletes p
  // p->drop(); // should create memory error 
  PASS;
}
TRACE(BadAllocException, "uncaught")

bool testConstructors() THROWS(BadAllocException) {
  vector<int> *vec;
  NEW(vec, vector<int>);
  PROG(vec != NULL);
  OPtr<vector<int> > *p;
  NEW(p, OPtr<vector<int> >, vec);
  OPtr<vector<int> > p3 (*p);
  OPtr<vector<int> > *p2;
  NEW(p2, OPtr<vector<int> >, &p3);
  PROG(p->ptr() == vec);
  PROG(p2->dptr() == vec);
  PROG(p3.dptr() == vec);
  p->hold();
  p2->drop(); // now p2 is invalid
  // vec is still valid
  PROG(p->dptr() == vec);
  PROG(p3.dptr() == vec);
  p->drop();
  p->drop(); // now p is invalid
  // vec is still valid
  PROG(p3.dptr() == vec);
  vector<int> *vec2;
  NEW(vec2, vector<int>);
  NEW(p, OPtr<vector<int> >, vec2); // p is valid
  p3 = p;
  // vec is now invalid due to no Ptr reference
  // vec->size(); // should create memory error
  PROG(p->dptr() == vec2);
  PROG(p3.dptr() == vec2);
  (*p)->push_back(5);
  p3->push_back(6);
  PROG(p3->at(1) == 6);
  PROG((*p)->at(0) == 5);
  PROG(p3->size() == 2);
  p->drop(); // now p is invalid
  PROG(p3.dptr() == vec2);
  // vec2 will be freed with p3
  PASS;
}
TRACE(BadAllocException, "uncaught")

int main (int argc, char **argv) {
  INIT;
  TEST(testVector);
  TEST(testConstructors);
  FINAL;
}
