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

#include "sys/ints.h"

namespace sys {

using namespace std;

// TODO check for common BYTE_ORDER macros

inline
bool is_big_endian() {
  static const union {
    uint32_t i;
    uint8_t c[4];
  } endint = { UINT32_C(0x01020304) };
  return endint.c[0] == 1;
}

inline
bool is_little_endian() {
  return !is_big_endian();
}

}
