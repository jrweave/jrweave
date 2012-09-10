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

#ifndef __LANG__MALFORMEDLANGTAGEXCEPTION_H__
#define __LANG__MALFORMEDLANGTAGEXCEPTION_H__

#include <cstring>
#include "ex/TraceableException.h"
#include "ptr/DPtr.h"
#include "sys/ints.h"
#include "ucs/UCSIter.h"

namespace lang {

using namespace ex;
using namespace ptr;
using namespace std;
using namespace ucs;

class MalformedLangTagException : public TraceableException {
public:
  MalformedLangTagException(const char *file, const unsigned int line) throw();
  MalformedLangTagException(const char *file, const unsigned int line, DPtr<uint8_t> *mal)
      throw();
  virtual ~MalformedLangTagException() throw();

  // Inherited Methods
  const char *what() const throw();
};

}

#endif /* __LANG__MALFORMEDLANGTAGEXCEPTION_H__ */
