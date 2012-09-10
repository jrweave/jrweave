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

#include "ptr/Ptr.h"
#include "test/unit.h"

#include "ptr/alloc.h"
#include "sys/ints.h"

using namespace ptr;
using namespace std;

bool testOverall() THROWS(BadAllocException) {
  uint8_t *vp;
  PROG(alloc(vp, 1));
  Ptr *p;
  NEW(p, Ptr, vp);
  PROG(p->ptr() == vp);
  int max = 10;
  int i;
  for (i = 0; i < max; i++) {
    p->hold();
  }
  for (i = 0; i < max; i++) {
    p->drop();
  }
  PROG(p->ptr() == vp);
  uint8_t *vp2;
  PROG(alloc(vp2, 1));
  *p = vp2;
  // vp is still a valid pointer
  dalloc(vp);
  PROG(p->ptr() != vp);
  PROG(p->ptr() == vp2);
  p->hold();
  p->drop();
  p->drop(); // this deletes p
  //p->drop(); // should create memory error 
  // vp2 is still a valid pointer
  dalloc(vp2);
  PASS;
}
TRACE(BadAllocException, "uncaught")

bool testConstructors() THROWS(BadAllocException) {
  PROG(true);
  uint8_t *vp;
  PROG(alloc(vp, 1));
  PROG(vp != NULL);
  Ptr *p;
  NEW(p, Ptr, vp);
  PROG(true);
  Ptr p3 (*p);
  PROG(true);
  Ptr *p2;
  NEW(p2, Ptr, &p3);
  PROG(p->ptr() == vp);
  PROG(p2->ptr() == vp);
  PROG(p3.ptr() == vp);
  p->hold();
  p2->drop(); // now p2 is invalid
  PROG(p->ptr() == vp);
  PROG(p3.ptr() == vp);
  p->drop();
  p->drop(); // now p is invalid
  PROG(p3.ptr() == vp);
  uint8_t *vp2;
  PROG(alloc(vp2, 1));
  NEW(p, Ptr, vp2);
  p3 = p;
  PROG(p->ptr() == vp2);
  PROG(p3.ptr() == vp2);
  p3 = vp;
  PROG(p3.ptr() == vp);
  p3 = *p;
  PROG(p3.ptr() == vp2);
  p->drop(); // now p is invalid
  PROG(p3.ptr() == vp2);
  // vp is still valid
  dalloc(vp);
  // vp2 will remain valid unless freed
  dalloc(vp2);
  PASS;
}
TRACE(BadAllocException, "uncaught")

int main (int argc, char **argv) {
  INIT;
  TEST(testOverall);
  TEST(testConstructors);
  FINAL;
}
