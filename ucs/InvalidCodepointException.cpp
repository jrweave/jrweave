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

#include "ucs/InvalidCodepointException.h"

#include <sstream>

namespace ucs {

using namespace ex;
using namespace std;

InvalidCodepointException::InvalidCodepointException(const char *file,
    const unsigned int line, const uint32_t codepoint) throw()
    : TraceableException(file, line, "Invalid codepoint."),
      codepoint(codepoint) {
  stringstream ss (stringstream::in | stringstream::out);
  ss << " caused by 0x" << hex << this->codepoint << "\n";
  this->stack_trace.append(ss.str());
}

InvalidCodepointException::~InvalidCodepointException() throw() {
  // do nothing
}

}
