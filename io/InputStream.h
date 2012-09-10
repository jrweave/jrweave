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

#ifndef __IO__INPUTSTREAM_H__
#define __IO__INPUTSTREAM_H__

#include "io/IOException.h"
#include "ptr/DPtr.h"
#include "sys/ints.h"

namespace io {

using namespace ex;
using namespace ptr;
using namespace std;

class InputStream {
public:
  virtual ~InputStream() throw(IOException);
  virtual int64_t available() throw(IOException);
  virtual void close() throw(IOException) = 0;
  virtual bool mark(const int64_t read_limit) throw(IOException);
  virtual bool markSupported() const throw();
  virtual DPtr<uint8_t> *read() throw(IOException, BadAllocException);
  virtual DPtr<uint8_t> *read(const int64_t amount)
      throw(IOException, BadAllocException) = 0;
  virtual void reset() throw(IOException);
  virtual int64_t skip(const int64_t n) throw(IOException);
};

}

#include "io/InputStream-inl.h"

#endif /* __IO__INPUTSTREAM_H__ */
