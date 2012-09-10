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

#include "lang/MalformedLangRangeException.h"

#include <string>
#include <iostream>
#include "ucs/utf.h"

namespace lang {

using namespace std;

MalformedLangRangeException::MalformedLangRangeException(const char *file,
    const unsigned int line) throw()
    : TraceableException(file, line, "Malformed LangRange") {
  // do nothing
}

MalformedLangRangeException::MalformedLangRangeException(const char *file,
    const unsigned int line, DPtr<uint8_t> *mal) throw()
    : TraceableException(file, line, "Malformed LangRange") {
  if (mal != NULL) {
    this->amendStackTrace(file, line,
                          string((char*) mal->ptr(), mal->size()).c_str());
  }
}

MalformedLangRangeException::~MalformedLangRangeException() throw() {
  // do nothing
}

const char *MalformedLangRangeException::what() const throw() {
  return TraceableException::what();
}

}
