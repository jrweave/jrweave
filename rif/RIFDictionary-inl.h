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

#include "rif/RIFDictionary.h"

namespace rif {

using namespace ptr;
using namespace std;

inline
RIFDictionary::RIFDictionary() throw()
    : term2id(Term2IDMap(RIFTerm::cmplt0)) {
  // do nothing
}

inline
RIFDictionary::RIFDictionary(const RIFDictionary &copy) throw()
    : term2id(copy.term2id), id2term(copy.id2term) {
  // do nothing
}

inline
RIFDictionary::~RIFDictionary() throw() {
  // do nothing
}

inline
enum RIFIDType RIFDictionary::getIDType(const uint64_t id) const throw() {
  return id >= RESERVED_IDS ? TERM :
        (id == UINT64_C(0) ? UNBOUND : ATOMIC_TYPE);
}

}
