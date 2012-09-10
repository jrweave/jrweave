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

#ifndef __UTIL__HASH_H__
#define __UTIL__HASH_H__

#include "sys/ints.h"

namespace util {

uint32_t hash_jenkins_one_at_a_time(const uint8_t *begin, const uint8_t *end)
    throw();

}

#include "util/hash-inl.h"

#endif /* __UTIL__HASH_H__ */
