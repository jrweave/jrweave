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

#ifndef __UTIL__FUNCS_H__
#define __UTIL__FUNCS_H__

#include <cstddef>

namespace util {

using namespace std;

template<typename T>
T &reverse_bytes(T &t);

}

#include "util/funcs-inl.h"

#endif /* __UTIL__FUNCS_H__ */
