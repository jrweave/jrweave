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

#ifndef __RIF__RIFATOMIC_H__
#define __RIF__RIFATOMIC_H__

#include <map>
#include "ptr/DPtr.h"
#include "rif/RIFTerm.h"

namespace rif {

using namespace ptr;
using namespace std;

enum RIFAtomicType {
  ATOM = 0,
  EXTERNAL = 1,
  EQUALITY = 2,
  MEMBERSHIP = 3,
  SUBCLASS = 4,
  FRAME = 5
};

class RIFAtomic;

typedef multimap<RIFTerm,
                 RIFTerm,
                 bool(*)(const RIFTerm &, const RIFTerm &)>
        SlotMap;

class RIFAtomic {
private:
  void *state;
  enum RIFAtomicType type;
  struct atom_state {
    RIFConst pred;
    DPtr<RIFTerm> *args;
    atom_state(const RIFConst &p, DPtr<RIFTerm> *a)
        : pred(p), args(a) {
      if (this->args != NULL) {
        if (!this->args->sizeKnown()) {
          THROWX(SizeUnknownException);
        }
        this->args->hold();
      }
    }
    atom_state(const atom_state &copy)
        : pred(copy.pred), args(copy.args) {
      if (this->args != NULL) {
        if (!this->args->sizeKnown()) {
          THROWX(SizeUnknownException);
        }
        this->args->hold();
      }
    }
    ~atom_state() {
      if (this->args != NULL) {
        this->args->drop();
      }
    }
    atom_state &operator=(const atom_state &rhs) {
      if (this == &rhs) {
        return *this;
      }
      if (this->args != NULL) {
        this->args->drop();
      }
      this->pred = rhs.pred;
      this->args = rhs.args;
      if (this->args != NULL) {
        this->args->hold();
      }
      return *this;
    }
  };
  struct frame_state {
    RIFTerm object;
    SlotMap slots;
    frame_state(const RIFTerm &obj, const SlotMap &avp)
        : object(obj), slots(avp) {
      // do nothing
    }
    frame_state(const frame_state &copy)
        : object(copy.object), slots(copy.slots) {
      // do nothing
    }
    ~frame_state() {
      // do nothing
    }
    frame_state &operator=(const frame_state &rhs) {
      if (this != &rhs) {
        this->object = rhs.object;
        this->slots = rhs.slots;
      }
      return *this;
    }
  };
public:
  RIFAtomic(const RIFConst &pred, DPtr<RIFTerm> *args, bool external)
      throw(SizeUnknownException, BadAllocException);
  RIFAtomic(const enum RIFAtomicType type, const RIFTerm &lhs,
      const RIFTerm &rhs)
      throw(BadAllocException, BaseException<enum RIFAtomicType>);
  RIFAtomic(const RIFTerm &obj, const SlotMap &slots)
      throw(BadAllocException);
  RIFAtomic(const RIFAtomic &copy) throw(BadAllocException);
  ~RIFAtomic() throw();
  
  static int cmp(const RIFAtomic &a1, const RIFAtomic &a2) throw();
  static bool cmplt0(const RIFAtomic &a1, const RIFAtomic &a2) throw();
  static bool cmpeq0(const RIFAtomic &a1, const RIFAtomic &a2) throw();
  static RIFAtomic parse(DPtr<uint8_t> *utf8str)
      throw(BadAllocException, SizeUnknownException,
            InvalidCodepointException, InvalidEncodingException,
            MalformedIRIRefException, TraceableException);

  DPtr<uint8_t> *toUTF8String() const throw(BadAllocException);
  bool equals(const RIFAtomic &rhs) const throw();
  RIFAtomic &normalize() throw(BadAllocException);
  enum RIFAtomicType getType() const throw();
  bool isGround() const throw();
  void getVars(VarSet &vars) const throw();

  // ATOM, EXTERNAL
  RIFConst getPred() const throw(BaseException<enum RIFAtomicType>);
  DPtr<RIFTerm> *getArgs() const throw(BaseException<enum RIFAtomicType>);

  // EQUALITY
  pair<RIFTerm, RIFTerm> getEquality() const
      throw(BaseException<enum RIFAtomicType>);

  // FRAME
  size_t getAttrs(TermSet &attrs) const
      throw(BaseException<enum RIFAtomicType>);
  size_t getValues(const RIFTerm &attr, TermSet &vals) const
      throw(BaseException<enum RIFAtomicType>);
  RIFAtomic withoutAttr(const RIFTerm &attr) const
      throw(BaseException<enum RIFAtomicType>);

  // MEMBERSHIP, FRAME
  RIFTerm getObject() const throw(BaseException<enum RIFAtomicType>);

  // MEMBERSHIP
  RIFTerm getClass() const throw(BaseException<enum RIFAtomicType>);

  // SUBCLASS
  RIFTerm getSubclass() const throw(BaseException<enum RIFAtomicType>);
  RIFTerm getSuperclass() const throw(BaseException<enum RIFAtomicType>);

  RIFAtomic &operator=(const RIFAtomic &rhs) throw(BadAllocException);
};

}

#include "rif/RIFAtomic-inl.h"

#endif /* __RIF__RIFATOMIC_H__ */
