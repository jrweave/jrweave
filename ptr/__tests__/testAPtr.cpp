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

#include "ptr/OPtr.h"
#include "test/unit.h"

using namespace ptr;
using namespace std;

bool testStand() THROWS(BadAllocException) {
  DPtr<int> *p1;
  NEW(p1, APtr<int>, 2);
  (*p1)[0] = 0;
  (*p1)[1] = 1;
  DPtr<int> *p2;
  NEW(p2, APtr<int>, (APtr<int>*)p1);
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
  PROG(p2->standable());
  p1->drop();
  int *x = p3->dptr();
  p3->drop();
  p2 = p2->stand();
  PROG(x + 1 != p2->dptr());
  p2->drop();
  PASS;
}
TRACE(BadAllocException, "(trace)")

bool testInt() THROWS(BadAllocException) {
  int val = -382;
  int *ip;
  NEW_ARRAY(ip, int, 2);
  *ip = val;
  ip[1] = val + 1;
  APtr<int> *p;
  NEW(p, APtr<int>, ip, 2);
  PROG((int*)p->ptr() == ip);
  PROG(p->dptr() == ip);
  PROG(**p == val);
  PROG((*p)[1] == val + 1);
  PROG(p->sizeKnown());
  PROG(p->size() == 2);
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
  NEW(p, APtr<int>, 1);
  **p = val;
  PROG((*p)[0] == val);
  (*p)[0] = 0;
  PROG((*p)[0] == 0);
  p->hold();
  p->drop();
  p->drop(); // this deletes p
  // p->drop(); // should create memory error 
  PASS;
}
TRACE(BadAllocException, "uncaught")

bool testConstructors() THROWS(BadAllocException) {
  char *vp;
  NEW_ARRAY(vp, char, 1);
  PROG(vp != NULL);
  APtr<char> *p;
  NEW(p, APtr<char>, vp);
  APtr<char> p3 (*p);
  APtr<char> *p2;
  NEW(p2, APtr<char>, &p3);
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
  NEW(p, APtr<char>, 5832);
  PROG(p->dptr() != p3.dptr());
  p3 = p;
  PROG(p->dptr() == p3.dptr());
  (*p)['a'] = 'a';
  p3['z'] = 'z';
  PROG(p3['z'] == 'z');
  PROG((*p)['a'] == 'a');
  p->drop(); // now p is invalid
  PASS;
}
TRACE(BadAllocException, "uncaught")

bool testArrayOfDPtrs() THROWS(BadAllocException) {
  const char *cstrs[5] = {
    "0", "one", "dos", "trois", "IV"
  };
  APtr<OPtr<string> > strings (5);
  PROG(strings.sizeKnown());
  PROG(strings.size() == 5);
  int i;
  for (i = 0; i < 5; i++) {
    string *s;
    NEW(s, string, cstrs[i]);
    strings[i] = s;
  }
  PROG(true);
  for (i = 0; i < 5; i++) {
    PROG(*(strings[i]) == string(cstrs[i]));
  }
  for (i = 0; i < 5; i++) {
    strings[i] = strings[4-i];
  }
  for (i = 0; i < 3; i++) {
    PROG(*(strings[i]) == string(cstrs[4-i]));
  }
  for (i = 2; i < 5; i++) {
    PROG(*(strings[i]) == string(cstrs[i]));
  }
  PASS;
}
TRACE(BadAllocException, "uncaught")

int main (int argc, char **argv) {
  INIT;
  TEST(testInt);
  TEST(testConstructors);
  TEST(testArrayOfDPtrs);
  TEST(testStand);
  FINAL;
}
