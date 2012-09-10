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

#ifndef __PTR__BADALLOCEXCEPTION_H__
#define __PTR__BADALLOCEXCEPTION_H__

#include <cstring>
#include <new>
#include "ex/TraceableException.h"

#define RETHROW_BAD_ALLOC \
  JUST_RETHROW(ptr::BadAllocException, "(rethrow)") \
  catch(std::bad_alloc &ba) { \
    THROWX(ptr::BadAllocException); \
  }

namespace ptr {

using namespace ex;
using namespace std;

class BadAllocException : public TraceableException {
private:
  size_t size;
  bool size_known;
public:
  BadAllocException(const char *file, const unsigned int line) throw();
  BadAllocException(const char *file, const unsigned int line, size_t size)
      throw();
  virtual ~BadAllocException() throw();

  // Inherited Methods
  const char *what() const throw();
};

}

#endif /* __PTR__BADALLOCEXCEPTION_H__ */
