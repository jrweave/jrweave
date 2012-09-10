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

#ifndef __EX__TRACEABLEEXCEPTION_H__
#define __EX__TRACEABLEEXCEPTION_H__

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#define THROWX(type) \
  throw type(__FILE__, __LINE__)

#define THROW(type, ...) \
  throw type(__FILE__, __LINE__, __VA_ARGS__) 

#define RETHROWX(ex) \
  ex.amendStackTrace(__FILE__, __LINE__); \
  throw ex

#define RETHROW(ex, ...) \
  ex.amendStackTrace(__FILE__, __LINE__, __VA_ARGS__); \
  throw ex

#define RETHROW_AS(type, e2) \
  THROW(type, e2.what())

#define THROWS(...) \
  throw(__VA_ARGS__) { \
    try

#define JUST_RETHROW(type, ...) \
  catch (type &_e) { \
    RETHROW(_e, __VA_ARGS__); \
  }

#define TRACE(type, ...) \
    JUST_RETHROW(type, __VA_ARGS__) \
  }

namespace ex {

using namespace std;

class TraceableException : public exception {
private:
  const char *file;
  const unsigned int line;
protected:
  string message, stack_trace;
  void init_stack_trace() throw();
public:
  TraceableException(const char *file, const unsigned int line)
      throw(const char *);
  TraceableException(const char *file, const unsigned int line,
      const char *message) throw(const char *);
  TraceableException(const TraceableException *) throw();
  virtual ~TraceableException() throw();
  
  // Final Methods
  const char *getFile() const throw();
  unsigned int getLine() const throw();
  const char *getMessage() const throw();
  TraceableException &amendStackTrace(const char *file,
      const unsigned int line) throw(const char *);
  TraceableException &amendStackTrace(const char *file,
      const unsigned int line, const char *message) throw(const char *);

  // Virtual Methods
  virtual const char *what() const throw();
};

}

std::ostream &operator<<(std::ostream &stream, ex::TraceableException *ex);

#endif /* __EX__TRACEABLEEXCEPTION_H__ */
