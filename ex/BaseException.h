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

#ifndef __EX__BASEEXCEPTIONS_H__
#define __EX__BASEEXCEPTIONS_H__

#include "ex/TraceableException.h"

namespace ex {

using namespace std;

template<typename cause_type>
class BaseException : public TraceableException {
private:
  const cause_type cause;
public:
  BaseException(const char *file, const unsigned int line,
      const cause_type cause) throw();
  BaseException(const char *file, const unsigned int line,
      const cause_type cause, const char *message) throw();
  BaseException(const BaseException *) throw();
  virtual ~BaseException() throw();

  // Final Methods
  const cause_type getCause() const throw();

  // Inherited Methods
  virtual const char *what() const throw();
};

}

#include "ex/BaseException-inl.h"

#endif /* __EX__BASEEXCEPTIONS_H__ */
