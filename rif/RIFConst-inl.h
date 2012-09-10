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

#include "rif/RIFConst.h"

namespace rif {

using namespace iri;
using namespace ptr;
using namespace std;

inline
RIFConst::RIFConst(const RIFConst &copy) throw()
    : lex(copy.lex), datatype(copy.datatype), normalized(copy.normalized) {
  this->lex->hold();
}

inline
RIFConst::~RIFConst() throw() {
  this->lex->drop();
}

inline
bool RIFConst::cmplt0(const RIFConst &rc1, const RIFConst &rc2) throw() {
  return RIFConst::cmp(rc1, rc2) < 0;
}

inline
bool RIFConst::cmpeq0(const RIFConst &rc1, const RIFConst &rc2) throw() {
  return RIFConst::cmp(rc1, rc2) == 0;
}

inline
bool RIFConst::equals(const RIFConst &rhs) const throw() {
  return RIFConst::cmp(*this, rhs) == 0;
}

inline
DPtr<uint8_t> *RIFConst::getLexForm() const throw() {
  this->lex->hold();
  return this->lex;
}

inline
IRIRef RIFConst::getDatatype() const throw() {
  return this->datatype;
}

}
