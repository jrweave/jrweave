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

#include "io/OFStream.h"

namespace io {

using namespace std;

inline
OFStream::OFStream(const char *filename) throw()
    : OStream<ofstream>() {
  this->stream.open(filename, ofstream::out | ofstream::binary);
  if (this->stream.fail()) {
    THROW(IOException, "Failed to open file.");
  }
}

inline
OFStream::~OFStream() throw(IOException) {
  // do nothing
}

inline
void OFStream::close() throw(IOException) {
  this->stream.close();
  if (this->stream.fail()) {
    THROW(IOException, "Failed to close ofstream.");
  }
}

}
