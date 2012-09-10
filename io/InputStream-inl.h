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

inline
int64_t InputStream::available() throw(IOException) {
  return INT64_C(0);
}

inline
bool InputStream::mark(const int64_t read_limit) throw(IOException) {
  return false;
}

inline
bool InputStream::markSupported() const throw() {
  return false;
}

inline
void InputStream::reset() throw(IOException) {
  THROW(IOException, "reset is not supported.");
}

}
