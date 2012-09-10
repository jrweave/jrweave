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

#include "rif/RIFCondition.h"

namespace rif {

using namespace ptr;
using namespace std;

inline
RIFCondition::RIFCondition() throw()
    : type(DISJUNCTION), state(NULL) {
  // do nothing; Or() is false
}

inline
bool RIFCondition::cmplt0(const RIFCondition &c1, const RIFCondition &c2)
    throw() {
  return RIFCondition::cmp(c1, c2) < 0;
}

inline
bool RIFCondition::cmpeq0(const RIFCondition &c1, const RIFCondition &c2)
    throw() {
  return RIFCondition::cmp(c1, c2) == 0;
}

inline
bool RIFCondition::equals(const RIFCondition &rhs) const throw() {
  return RIFCondition::cmp(*this, rhs) == 0;
}

inline
enum RIFCondType RIFCondition::getType() const throw() {
  return this->type;
}

inline
bool RIFCondition::isGround() const throw() {
  VarSet vars(RIFVar::cmplt0);
  this->getVars(vars);
  return vars.size() == 0;
}

}
