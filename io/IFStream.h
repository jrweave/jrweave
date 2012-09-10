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

#ifndef __IO__IFSTREAM_H__
#define __IO__IFSTREAM_H__

#include <fstream>
#include "io/IStream.h"

namespace io {

using namespace ex;
using namespace ptr;
using namespace std;

class IFStream : public IStream<ifstream> {
public:
  IFStream(const char *filename) throw(BadAllocException, IOException);
  IFStream(const char *filename, const size_t bufsize)
      throw(BadAllocException, IOException);
  virtual ~IFStream() throw(IOException);
  virtual void close() throw(IOException);
};

}

#include "IFStream-inl.h"

#endif /* __IO__IFSTREAM_H__ */
