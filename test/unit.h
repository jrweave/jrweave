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

#ifndef __TEST__UNIT_H__
#define __TEST__UNIT_H__

#include <iostream>

#ifdef PTR_MEMDEBUG
#include "ptr/alloc.h"
#endif

#define INIT \
  unsigned int __ntests = 0; \
  unsigned int __failures = 0

#define TEST(call, ...) \
  std::cerr << "TEST " #call "(" #__VA_ARGS__ ")\n"; \
  if (!call(__VA_ARGS__)) __failures++; \
  __ntests++

#define PASS \
  std::cerr << "\nPASSED\n"; \
  return true

#define FAIL \
  std::cerr << std::dec << __LINE__ << " FAILED!\n"; \
  std::cerr << "+---------------------------+\n"; \
  std::cerr << "| XXXXX  XXX  XXXXX X     X |\n"; \
  std::cerr << "| X     X   X   X   X     X |\n"; \
  std::cerr << "| XXX   XXXXX   X   X     X |\n"; \
  std::cerr << "| X     X   X   X   X       |\n"; \
  std::cerr << "| X     X   X XXXXX XXXXX X |\n"; \
  std::cerr << "+---------------------------+\n"; \
  return false

#define PROG(cond) \
  if (!(cond)) { FAIL; } \
  std::cerr << std::dec << __LINE__ << ","

#define MARK(msg) \
  std::cerr << "MARK " << __LINE__ << " " << msg << std::endl

#ifndef PTR_MEMDEBUG
#define FINAL \
  std::cerr << std::dec; \
  if (__failures > 0) { \
    std::cerr << "FAILED " << __failures << " OUT OF " << __ntests << " TESTS!\n"; \
    std::cerr << "+---------------------------+\n"; \
    std::cerr << "| XXXXX  XXX  XXXXX X     X |\n"; \
    std::cerr << "| X     X   X   X   X     X |\n"; \
    std::cerr << "| XXX   XXXXX   X   X     X |\n"; \
    std::cerr << "| X     X   X   X   X       |\n"; \
    std::cerr << "| X     X   X XXXXX XXXXX X |\n"; \
    std::cerr << "+---------------------------+\n"; \
    exit(__failures); \
  } \
  std::cerr << "CLEARED ALL " << __ntests << " TESTS\n"; \
  exit(0)
#else
#define FINAL \
  std::cerr << std::dec; \
  if (__failures > 0) { \
    std::cerr << "FAILED " << __failures << " OUT OF " << __ntests << " TESTS!\n"; \
    std::cerr << "+---------------------------+\n"; \
    std::cerr << "| XXXXX  XXX  XXXXX X     X |\n"; \
    std::cerr << "| X     X   X   X   X     X |\n"; \
    std::cerr << "| XXX   XXXXX   X   X     X |\n"; \
    std::cerr << "| X     X   X   X   X       |\n"; \
    std::cerr << "| X     X   X XXXXX XXXXX X |\n"; \
    std::cerr << "+---------------------------+\n"; \
    exit(__failures); \
  } \
  if (ptr::__PTRS.size() > 0) { \
    std::set<void*>::iterator __it; \
    for (__it = ptr::__PTRS.begin(); __it != ptr::__PTRS.end(); ++__it) { \
      std::cerr << *__it << std::endl; \
    } \
    std::cerr << "THERE ARE " << ptr::__PTRS.size() << " HANGING POINTERS!\n"; \
    std::cerr << "+-------------------------------------+\n"; \
    std::cerr << "| XXXXX XXXXX X   X     XXXXX XXXXX X |\n"; \
    std::cerr << "| X       X    X X        X     X   X |\n"; \
    std::cerr << "| XXX     X     X         X     X   X |\n"; \
    std::cerr << "| X       X    X X        X     X     |\n"; \
    std::cerr << "| X     XXXXX X   X     XXXXX   X   X |\n"; \
    std::cerr << "+-------------------------------------+\n"; \
    exit(ptr::__PTRS.size()); \
  } \
  std::cerr << "CLEARED ALL " << __ntests << " TESTS\n"; \
  exit(0)
#endif

#endif /* __TEST__UNIT_H__ */
