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
#include "test/unit.h"

#include "ptr/alloc.h"

using namespace ptr;
using namespace std;

bool testStand() THROWS(BadAllocException) {
  int *p;
  PROG(alloc(p, 2));
  DPtr<int> *p1;
  NEW(p1, DPtr<int>, p, 2);
  (*p1)[0] = 0;
  (*p1)[1] = 1;
  DPtr<int> *p2;
  NEW(p2, DPtr<int>, (DPtr<int>*)p1);
  DPtr<int> *p3 = p2;
  p3->hold();
  PROG(!p1->standable());
  PROG(!p2->standable());
  PROG(!p3->standable());
  PROG(NULL == p2->stand());
  dalloc(p);
  PASS;
}
TRACE(BadAllocException, "(trace)")

bool testInt() THROWS(BadAllocException) {
  int val = -382;
  int *ip;
  PROG(alloc(ip, 2));
  *ip = val;
  ip[1] = val + 1;
  DPtr<int> *p;
  NEW(p, DPtr<int>, ip);
  PROG((int*)p->ptr() == ip);
  PROG(p->dptr() == ip);
  PROG(**p == val);
  PROG((*p)[1] == val + 1);
  PROG(!p->sizeKnown());
  // p->func(); shouldn't compile
  int max = 10;
  int i;
  for (i = 0; i < max; i++) {
    p->hold();
  }
  for (i = 0; i < max; i++) {
    p->drop();
  }
  PROG(p->dptr() == ip);
  int *ip2;
  PROG(alloc(ip2, 1));
  ip2[0] = val + 2;
  *p = ip2;
  // ip is still a valid pointer
  dalloc(ip);
  PROG(p->dptr() != ip);
  PROG(p->dptr() == ip2);
  PROG(**p == val + 2);
  p->hold();
  p->drop();
  p->drop(); // this deletes p
  // p->drop(); // should create memory error 
  // ip2 is still a valid pointer
  dalloc(ip2);
  PASS;
}
TRACE(BadAllocException, "uncaught")

bool testConstructors() THROWS(BadAllocException) {
  char *vp;
  PROG(alloc(vp, 1));
  PROG(vp != NULL);
  DPtr<char> *p;
  NEW(p, DPtr<char>, vp);
  DPtr<char> p3 (*p);
  DPtr<char> *p2;
  NEW(p2, DPtr<char>, &p3);
  PROG(p->ptr() == vp);
  PROG(p2->dptr() == vp);
  PROG(p3.dptr() == vp);
  p->hold();
  p2->drop(); // now p2 is invalid
  PROG(p->dptr() == vp);
  PROG(p3.dptr() == vp);
  p->drop();
  p->drop(); // now p is invalid
  PROG(p3.dptr() == vp);
  char *vp2;
  PROG(alloc(vp2, 11));
  NEW(p, DPtr<char>, vp2);
  p3 = p;
  PROG(p->dptr() == vp2);
  PROG(p3.dptr() == vp2);
  p3 = vp;
  PROG(p3.dptr() == vp);
  p3 = *p;
  PROG(p3.dptr() == vp2);
  *(p->dptr()) = 5;
  *(p->dptr()) += 6;
  PROG(**p == 11);
  PROG(*p3 == 11);
  p->drop(); // now p is invalid
  PROG(p3.dptr() == vp2);
  // vp is still valid
  dalloc(vp);
  // vp2 will remain valid unless freed
  dalloc(vp2);
  PASS;
}
TRACE(BadAllocException, "uncaught")

int main (int argc, char **argv) {
  INIT;
  TEST(testInt);
  TEST(testConstructors);
  FINAL;
}
