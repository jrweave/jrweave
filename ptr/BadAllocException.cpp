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

#include "ptr/BadAllocException.h"

#include <sstream>

namespace ptr {

using namespace std;

BadAllocException::BadAllocException(const char *file,
    const unsigned int line) throw()
    : TraceableException(file, line, "Bad Allocation"), size(0),
      size_known(false) {
  // do nothing
}

BadAllocException::BadAllocException(const char *file,
    const unsigned int line, size_t size) throw()
    : TraceableException(file, line, "Bad Allocation"), size(size),
      size_known(true) {
  // do nothing
}

BadAllocException::~BadAllocException() throw() {
  // do nothing
}

const char *BadAllocException::what() const throw() {
  if (!this->size_known) {
    return TraceableException::what();
  }
  stringstream ss (stringstream::in | stringstream::out);
  ss << TraceableException::what() << "\tcaused by request for "
      << this->size << " bytes\n";
  return ss.str().c_str();
}

}
