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

#include "util/funcs.h"

#include <cstring>
#include "sys/ints.h"

namespace util {

using namespace std;

template<typename T>
T &reverse_bytes(T &t) {
  const size_t tsize = sizeof(T) / sizeof(uint8_t);
  const size_t max = tsize >> 1;
  uint8_t *p = (uint8_t *) &t;
  size_t i;
  for (i = 0; i < max; i++) {
    size_t j = tsize - i - 1;
    const uint8_t temp = p[i];
    p[i] = p[j];
    p[j] = temp;
  }
  return t;
}

}
