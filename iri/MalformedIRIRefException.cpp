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

#include "iri/MalformedIRIRefException.h"

#include <string>
#include <iostream>
#include <sstream>
#include "ucs/utf.h"

namespace iri {

using namespace std;

MalformedIRIRefException::MalformedIRIRefException(const char *file,
    const unsigned int line) throw()
    : TraceableException(file, line, "Malformed IRIRef") {
  // do nothing
}

MalformedIRIRefException::MalformedIRIRefException(const char *file,
    const unsigned int line, UCSIter *mal) throw()
    : TraceableException(file, line, "Malformed IRIRef") {
  if (mal != NULL) {
    string str;
    uint8_t buf[4];
    mal->start();
    while (mal->more()) {
      size_t len = mal->next(buf);
      size_t i;
      for (i = 0; i < len; i++) {
        str.push_back((char) buf[i]);
      }
    }
    this->amendStackTrace(file, line, str.c_str());
    DELETE(mal);
  }
}

MalformedIRIRefException::~MalformedIRIRefException() throw() {
  // do nothing
}

const char *MalformedIRIRefException::what() const throw() {
  return TraceableException::what();
}

}
