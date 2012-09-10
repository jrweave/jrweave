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

#include <sstream>

#include <cstring>
#include "ex/TraceableException.h"

namespace ex {

using namespace std;

TraceableException::TraceableException(const char *file,
    const unsigned int line) throw(const char *)
    : file(file), line(line) {
  if (file == NULL) {
    throw file;
  }
  this->init_stack_trace();
}

TraceableException::TraceableException(const char *file,
    const unsigned int line, const char *message) throw(const char *) 
    : file(file), line(line) {
  if (file == NULL) {
    throw file;
  }
  if (message != NULL) {
    this->message = string(message);
  }
  this->init_stack_trace();
}

TraceableException::TraceableException(const TraceableException *ex) throw ()
    : file(ex->file), line(ex->line), message(ex->message),
      stack_trace(ex->stack_trace) {
  // do nothing
}

TraceableException::~TraceableException() throw() {
  // do nothing
}

void TraceableException::init_stack_trace() throw() {
  stringstream ss (stringstream::in | stringstream::out);
  ss << this->file << ":" << this->line << ": " << this->message << endl;
  this->stack_trace = ss.str();
}

const char *TraceableException::getFile() const throw() {
  return this->file;
}

unsigned int TraceableException::getLine() const throw() {
  return this->line;
}

const char *TraceableException::getMessage() const throw() {
  return this->message.c_str();
}

TraceableException &TraceableException::amendStackTrace(const char *file,
    const unsigned int line) throw(const char *) {
  return this->amendStackTrace(file, line, NULL);
}

TraceableException &TraceableException::amendStackTrace(const char *file,
    const unsigned int line, const char *message) throw(const char *) {
  if (file == NULL) {
    throw file;
  }
  stringstream ss (stringstream::in | stringstream::out);
  ss << "\t" << file << ":" << line;
  if (message != NULL) {
    ss << ": " << message;
  }
  ss << endl;
  this->stack_trace.append(ss.str());
  return *this;
}

const char *TraceableException::what() const throw() {
  return this->stack_trace.c_str();
}

}

std::ostream &operator<<(std::ostream &stream, ex::TraceableException *ex) {
  return stream << ex->what();
}
