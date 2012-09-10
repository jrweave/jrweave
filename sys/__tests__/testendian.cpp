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

#include "sys/endian.h"

#include "sys/sys.h"
#include "test/unit.h"

using namespace std;
using namespace sys;

bool testEndianess() {
#if SYSTEM == SYS_MACBOOK_PRO
  PROG(is_little_endian());
  PROG(!is_big_endian());
  PASS;
#else
#error "SYSTEM must be specified to a known value for this test.\n"
#endif
}

int main(int argc, char **argv) {
  INIT;
  TEST(testEndianess);
  FINAL;
}
