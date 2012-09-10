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

#include "ex/BaseException.h"

#include <sstream>

namespace ex {

using namespace std;

template<typename cause_type>
BaseException<cause_type>::BaseException(const char *file,
    const unsigned int line, const cause_type cause) throw()
    : TraceableException(file, line), cause(cause) {
  stringstream ss (stringstream::in | stringstream::out);
  ss << " caused by " << this->cause << endl;
  this->stack_trace.append(ss.str());
}

template<typename cause_type>
BaseException<cause_type>::BaseException(const char *file,
    const unsigned int line, const cause_type cause,
    const char *message) throw()
    : TraceableException(file, line, message), cause(cause) {
  stringstream ss (stringstream::in | stringstream::out);
  ss << " caused by " << this->cause << endl;
  this->stack_trace.append(ss.str());
}

template<typename cause_type>
BaseException<cause_type>::BaseException(const BaseException *ex) throw()
    : TraceableException(ex), cause(ex->cause) {
  // do nothing
}

template<typename cause_type>
BaseException<cause_type>::~BaseException() throw() {
  // do nothing
}

template<typename cause_type>
const cause_type BaseException<cause_type>::getCause() const throw() {
  return this->cause;
}

template<typename cause_type>
const char *BaseException<cause_type>::what() const throw() {
  return this->stack_trace.c_str();
}

}
