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

#include "io/IFStream.h"

namespace io {

inline
IFStream::IFStream(const char *filename)
    throw(BadAllocException, IOException)
    : IStream<ifstream>(4096) {
  this->stream.open(filename, ifstream::in | ifstream::binary);
  if (this->stream.fail()) {
    THROW(IOException, "Failed to open file.");
  }
}

inline
IFStream::IFStream(const char *filename, const size_t bufsize)
    throw(BadAllocException, IOException)
    : IStream<ifstream>(bufsize) {
  this->stream.open(filename, ifstream::in | ifstream::binary);
  if (this->stream.fail()) {
    THROW(IOException, "Failed to open file.");
  }
}

inline
IFStream::~IFStream() throw(IOException) {
  // do nothing; superclass destructor closes
}

inline
void IFStream::close() throw(IOException) {
  this->stream.close();
}

}
