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

#include "rif/RIFTerm.h"

namespace rif {

inline
RIFTerm::RIFTerm() throw()
    : state(NULL), type(LIST) {
  // do nothing
}

inline
bool RIFTerm::cmplt0(const RIFTerm &t1, const RIFTerm &t2) throw() {
  return RIFTerm::cmp(t1, t2) < 0;
}

inline
bool RIFTerm::cmpeq0(const RIFTerm &t1, const RIFTerm &t2) throw() {
  return RIFTerm::cmp(t1, t2) == 0;
}

inline
bool RIFTerm::equals(const RIFTerm &rhs) const throw() {
  return RIFTerm::cmp(*this, rhs) == 0;
}

inline
enum RIFTermType RIFTerm::getType() const throw() {
  return this->type;
}

inline
bool RIFTerm::isSimple() const throw() {
  return this->type == VARIABLE || this->type == CONSTANT;
}

}
