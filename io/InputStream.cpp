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

#include "io/InputStream.h"

namespace io {

InputStream::~InputStream() throw(IOException) {
  // do nothing
}

DPtr<uint8_t> *InputStream::read() 
    throw(IOException, BadAllocException) {
  try {
    int64_t avail = this->available();
    return this->read(avail == 0 ? 1 : avail);
  } JUST_RETHROW(IOException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}

int64_t InputStream::skip(const int64_t n) throw(IOException) {
  int64_t i = 0;
  for (; i < n; ++i) {
    if (this->read() < INT16_C(0)) {
      return i == 0 ? INT64_C(-1) : i;
    }
  }
  return i;
}

}
