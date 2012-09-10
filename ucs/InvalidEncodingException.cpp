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

#include "ucs/InvalidEncodingException.h"

#include <sstream>
#include "sys/ints.h"

namespace ucs {

using namespace ex;
using namespace std;

InvalidEncodingException::InvalidEncodingException(const char *file,
    const unsigned int line, const char *message, const uint32_t encoded)
    throw()
    : TraceableException(file, line, message), encoded(encoded) { 
  stringstream ss (stringstream::in | stringstream::out);
  ss << " caused by invalid encoding 0x" << hex << this->encoded << "\n";
  this->stack_trace.append(ss.str());
}

InvalidEncodingException::~InvalidEncodingException() throw() {
  // do nothing
}

}
