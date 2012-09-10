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

#ifndef __IO__OUTPUTSTREAM_H__
#define __IO__OUTPUTSTREAM_H__

#include "ex/BaseException.h"
#include "io/IOException.h"
#include "ptr/DPtr.h"
#include "ptr/SizeUnknownException.h"
#include "sys/ints.h"

namespace io {

using namespace ex;
using namespace ptr;
using namespace std;

class OutputStream {
public:
  virtual ~OutputStream() throw(IOException);
  virtual void close() throw(IOException) = 0;
  virtual void flush() throw(IOException) = 0;
  virtual void write(DPtr<uint8_t> *buf)
      throw(IOException, SizeUnknownException,
            BaseException<void*>);
  virtual void write(DPtr<uint8_t> *buf, size_t &nwritten)
      throw(IOException, SizeUnknownException,
            BaseException<void*>) = 0;
};

}

#endif /* __IO__OUTPUTSTREAM_H__ */
