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

#ifndef __RIF__RIFCONDITION_H__
#define __RIF__RIFCONDITION_H__

#include "rif/RIFAtomic.h"

namespace rif {

using namespace ptr;
using namespace std;

enum RIFCondType {
  ATOMIC = 0,
  CONJUNCTION = 1,
  DISJUNCTION = 2,
  NEGATION = 3,
  EXISTENTIAL = 4
};

class RIFCondition {
private:
  void *state;
  // ATOMIC: RIFAtomic
  // CONJUNCTION: DPtr<RIFCondition>, possibly NULL
  // DISJUNCTION: DPtr<RIFCondition>, possibly NULL
  // NEGATION: RIFCondition
  // EXISTENTIAL: exist_state
  enum RIFCondType type;
public:
  RIFCondition() throw(); // Or()
  RIFCondition(const RIFAtomic &atomic) throw(BadAllocException);
  RIFCondition(const enum RIFCondType type, DPtr<RIFCondition> *subs)
      throw(SizeUnknownException, BaseException<enum RIFCondType>);
  RIFCondition(const RIFCondition &copy, bool dont_negate)
      throw(BadAllocException);
  RIFCondition(DPtr<RIFVar> *vars, const RIFCondition &sub)
      throw(BaseException<void*>, SizeUnknownException, TraceableException);
  RIFCondition(const RIFCondition &copy) throw(BadAllocException);
  ~RIFCondition() throw();

  static int cmp(const RIFCondition &c1, const RIFCondition &c2) throw();
  static bool cmplt0(const RIFCondition &c1, const RIFCondition &c2) throw();
  static bool cmpeq0(const RIFCondition &c1, const RIFCondition &c2) throw();
  static RIFCondition parse(DPtr<uint8_t> *utf8str)
      throw(BadAllocException, SizeUnknownException,
            InvalidCodepointException, InvalidEncodingException,
            MalformedIRIRefException, TraceableException);

  DPtr<uint8_t> *toUTF8String() const throw(BadAllocException);
  bool equals(const RIFCondition &rhs) const throw();
  RIFCondition &normalize() throw(BadAllocException);
  enum RIFCondType getType() const throw();
  bool isGround() const throw();
  void getVars(VarSet &vars) const throw();

  // ATOMIC
  RIFAtomic getAtomic() const throw(BaseException<enum RIFCondType>);

  // CONJUNCTION, DISJUNCTION
  DPtr<RIFCondition> *getSubformulas() const
      throw(BaseException<enum RIFCondType>);

  // NEGATION, EXISTENTIAL
  RIFCondition getSubformula() const throw(BaseException<enum RIFCondType>);

  RIFCondition &operator=(const RIFCondition &rhs) throw(BadAllocException);
};

// TODO this should really be made private somehow
struct exist_state {
  DPtr<RIFVar> *vars;
  RIFCondition sub;
  exist_state(DPtr<RIFVar> *vars, const RIFCondition &c)
      : vars(vars), sub(c) {
    if (this->vars == NULL) {
      THROW(BaseException<void*>, NULL,
            "Existential formula must have at least one variable.");
    }
    if (!this->vars->sizeKnown()) {
      THROWX(SizeUnknownException);
    }
    if (this->vars->size() <= 0) {
      THROW(TraceableException,
            "Existential formula must have at least one variable.");
    }
    this->vars->hold();
  }
  exist_state(const exist_state &copy)
      : vars(copy.vars), sub(copy.sub) {
    this->vars->hold();
  }
  ~exist_state() {
    this->vars->drop();
  }
  exist_state &operator=(const exist_state &rhs) {
    if (this != &rhs) {
      this->sub = rhs.sub;
      this->vars->drop();
      this->vars = rhs.vars;
      this->vars->hold();
    }
    return *this;
  }
};

}

#include "rif/RIFCondition-inl.h"

#endif /* __RIF__RIFCONDITION_H__ */
