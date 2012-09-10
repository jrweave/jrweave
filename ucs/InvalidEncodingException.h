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

#ifndef __UCS__INVALIDENCODINGEXCEPTION_H__
#define __UCS__INVALIDENCODINGEXCEPTION_H__

#include "ex/TraceableException.h"
#include "sys/ints.h"

namespace ucs {

using namespace ex;
using namespace std;

class InvalidEncodingException : public TraceableException {
private:
  const uint32_t encoded;
public:
  InvalidEncodingException(const char *file, const unsigned int line,
    const char *message, const uint32_t encoded) throw();
  virtual ~InvalidEncodingException() throw();
};

}

#endif /* __UCS__INVALIDENCODINGEXCEPTION_H__ */
