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

#include "io/OutputStream.h"

namespace io {

OutputStream::~OutputStream() throw(IOException) {
  // do nothing
}

void OutputStream::write(DPtr<uint8_t> *buf)
    throw(IOException, SizeUnknownException, BaseException<void*>) {
  size_t nwritten;
  try {
    this->write(buf, nwritten);
  } JUST_RETHROW(IOException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BaseException<void*>, "(rethrow)")
}

}
