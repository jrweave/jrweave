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

#ifndef __RIF__RIFDICTIONARY_H__
#define __RIF__RIFDICTIONARY_H__

#include <map>
#include "ptr/DPtr.h"
#include "rif/RIFAtomic.h"
#include "sys/ints.h"

namespace rif {

using namespace ptr;
using namespace std;

enum RIFIDType {
  UNBOUND = 0,
  ATOMIC_TYPE = 1,
  TERM = 2
};

typedef map<RIFTerm,
            uint64_t,
            bool(*)(const RIFTerm &, const RIFTerm &)>
        Term2IDMap;

typedef map<uint64_t,
            RIFTerm>
        ID2TermMap;

class RIFDictionary {
private:
  Term2IDMap term2id;
  ID2TermMap id2term;
  static const uint64_t RESERVED_IDS = UINT64_C(6);
    // 0: USED FOR MARKING UNBOUND PARTS (VARIABLES)
    // 1: RIFAtomicType ATOM
    // 2-5: RIFAtomicType
public:
  RIFDictionary() throw();
  RIFDictionary(const RIFDictionary &copy) throw();
  ~RIFDictionary() throw();

  enum RIFIDType getIDType(const uint64_t id) const throw();

  uint64_t encode(const enum RIFAtomicType type) const
    throw(BaseException<enum RIFAtomicType>);
  uint64_t encode(const RIFTerm &term) throw(BaseException<enum RIFTermType>);
  DPtr<uint64_t> *encode(const RIFAtomic &atomic)
    throw(BadAllocException, BaseException<enum RIFAtomicType>,
          BaseException<enum RIFTermType>);
  enum RIFAtomicType decodeType(const uint64_t id) const
    throw(BaseException<uint64_t>);
  RIFTerm decodeTerm(const uint64_t id) const throw(BaseException<uint64_t>);
  RIFAtomic decode(DPtr<uint64_t> *ids, map<size_t, RIFVar> *vars) const
    throw(BaseException<void*>, SizeUnknownException, BaseException<uint64_t>,
          TraceableException);

  RIFDictionary &operator=(const RIFDictionary &rhs) throw();
};

}

#include "rif/RIFDictionary-inl.h"

#endif /* __RIF__RIFDICTIONARY_H__ */
