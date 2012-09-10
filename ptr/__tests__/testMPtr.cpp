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

#include "ptr/MPtr.h"
#include "test/unit.h"

#include <iostream>

using namespace ptr;
using namespace std;

bool testStandSimple() THROWS(BadAllocException) {
  DPtr<int> *p1;
  NEW(p1, MPtr<int>, 2);
  DPtr<int> *p2 = p1->sub(1, p1->size() - 1);
  PROG(p1->standable());
  PROG(!p2->alone());
  p2 = p2->stand();
  PROG(p2->alone());
  p1->drop();
  p2->drop();
  PASS;
}
TRACE(BadAllocException, "(trace)")

bool testStand() THROWS(BadAllocException) {
  DPtr<int> *p1;
  NEW(p1, MPtr<int>, 2);
  (*p1)[0] = 0;
  (*p1)[1] = 1;
  DPtr<int> *p2;
  NEW(p2, MPtr<int>, (MPtr<int>*)p1);
  DPtr<int> *p3 = p2;
  p3->hold();
  PROG(p1->standable());
  PROG(p2->standable());
  PROG(p3->standable());
  p2 = p2->stand();
  PROG(p1->ptr() == p3->ptr());
  PROG(p1->ptr() != p2->ptr());
  PROG(p1->size() == p2->size());
  PROG(memcmp(p1->dptr(), p2->dptr(), 2*sizeof(int)) == 0);
  p2->drop();
  p2 = p1->sub(1, p1->size() - 1);
  PROG(p3->dptr() + 1 == p2->dptr());
  PROG(p2->standable());
  p1->drop();
  p2 = p2->stand();
  cerr << "HEY " << p3->dptr() + 1 << " " << p2->dptr() << endl;
  PROG(p3->dptr() + 1 != p2->dptr());
  PROG(p3->size() - 1 == p2->size());
  p2->drop();
  p3->drop();
  PASS;
}
TRACE(BadAllocException, "(trace)")

bool testInt() THROWS(BadAllocException) {
  int val = -382;
  int *ip;
  PROG(alloc(ip, 2));
  *ip = val;
  ip[1] = val + 1;
  MPtr<int> *p;
  NEW(p, MPtr<int>, ip, 2);
  PROG((int*)p->ptr() == ip);
  PROG(p->dptr() == ip);
  PROG(**p == val);
  PROG((*p)[1] == val + 1);
  PROG(p->sizeKnown());
  PROG(p->size() == 2);
  PROG(p->sizeInBytes() == 2 * sizeof(int));
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
  p->drop(); // p is invalid
  //p->hold(); // memory error
  NEW(p, MPtr<int>, 1);
  **p = val;
  PROG((*p)[0] == val);
  p->hold();
  p->drop();
  p->drop(); // this deletes p
  //p->drop(); // should create memory error 
  PASS;
}
TRACE(BadAllocException, "uncaught")

bool testConstructors() THROWS(BadAllocException) {
  char *vp;
  PROG(alloc(vp, 1));
  PROG(vp != NULL);
  MPtr<char> *p;
  NEW(p, MPtr<char>, vp, 1);
  MPtr<char> p3 (*p);
  MPtr<char> *p2;
  NEW(p2, MPtr<char>, &p3);
  PROG(p->ptr() == vp);
  PROG(p->sizeKnown());
  PROG(p->size() == 1);
  PROG(p2->dptr() == vp);
  PROG(p2->sizeKnown());
  PROG(p2->size() == 1);
  PROG(p3.dptr() == vp);
  PROG(p3.sizeKnown());
  PROG(p3.size() == 1);
  p->hold();
  p2->drop(); // now p2 is invalid
  PROG(p->dptr() == vp);
  PROG(p3.dptr() == vp);
  p->drop();
  p->drop(); // now p is invalid
  PROG(p3.dptr() == vp);
  NEW(p, MPtr<char>, 5832);
  PROG(p->dptr() != p3.dptr());
  p3 = p;
  PROG(p->dptr() == p3.dptr());
  PROG(p3.sizeKnown());
  PROG(p3.size() == p->size());
  (*p)['a'] = 'a';
  p3['z'] = 'z';
  PROG(p3['z'] == 'z');
  PROG((*p)['a'] == 'a');
  p->drop(); // now p is invalid
  PASS;
}
TRACE(BadAllocException, "uncaught")

int main (int argc, char **argv) {
  INIT;
  TEST(testInt);
  TEST(testConstructors);
  TEST(testStandSimple);
  TEST(testStand);
  FINAL;
}
