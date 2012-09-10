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

#include "rif/RIFAtomic.h"

namespace rif {

using namespace std;

inline
bool RIFAtomic::cmplt0(const RIFAtomic &a1, const RIFAtomic &a2) throw() {
  return RIFAtomic::cmp(a1, a2) < 0;
}

inline
bool RIFAtomic::cmpeq0(const RIFAtomic &a1, const RIFAtomic &a2) throw() {
  return RIFAtomic::cmp(a1, a2) == 0;
}

inline
bool RIFAtomic::equals(const RIFAtomic &rhs) const throw() {
  return RIFAtomic::cmp(*this, rhs) == 0;
}

inline
enum RIFAtomicType RIFAtomic::getType() const throw() {
  return this->type;
}


}
