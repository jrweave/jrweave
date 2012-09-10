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

#ifndef __RIF__RIFTERM_H__
#define __RIF__RIFTERM_H__

#include <map>
#include <set>
#include "ptr/BadAllocException.h"
#include "ptr/DPtr.h"
#include "rif/RIFConst.h"
#include "rif/RIFVar.h"

namespace rif {

using namespace ptr;
using namespace rif;
using namespace std;

enum RIFTermType {
  VARIABLE = 0,
  CONSTANT = 1,
  LIST = 2,
  FUNCTION = 3
};

enum RIFContext {
  INDIVIDUAL_NAME = 0,
  PREDICATE_NAME = 1,
  EXTERNAL_NAME = 2,
  FUNCTION_NAME = 3
};

class RIFTerm;

typedef set<RIFTerm,
            bool(*)(const RIFTerm &, const RIFTerm &)>
        TermSet;

typedef set<RIFVar,
            bool(*)(const RIFVar &, const RIFVar &)>
        VarSet;

class RIFTerm {
private:
  void *state; // NULL == empty list
  enum RIFTermType type;
  // CONSTANT ==> RIFConst
  // VARIABLE ==> RIFVar
  // LIST ==> DPtr<RIFTerm>
  // FUNCTION ==> func_state
  struct func_state {
    RIFConst pred;
    DPtr<RIFTerm> *args;
    func_state(const RIFConst &pred, DPtr<RIFTerm> *args)
        : pred(pred), args(args) {
      if (args != NULL) {
        if (!args->sizeKnown()) {
          THROWX(SizeUnknownException);
        }
        args->hold();
      }
    }
    func_state(const func_state &f)
        : pred(f.pred), args(f.args) {
      if (this->args != NULL) {
        args->hold();
      }
    }
    ~func_state() {
      if (this->args != NULL) {
        args->drop();
      }
    }
    func_state &operator=(const func_state &rhs) throw() {
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
public:
  RIFTerm() throw(); // List()
  RIFTerm(const RIFConst &c) throw(BadAllocException);
  RIFTerm(const RIFVar &v) throw(BadAllocException);
  RIFTerm(DPtr<RIFTerm> *list) throw(SizeUnknownException, TraceableException);
  RIFTerm(const RIFConst &pred, DPtr<RIFTerm> *args)
      throw(SizeUnknownException, BadAllocException);
  RIFTerm(const RIFTerm &copy) throw(BadAllocException);
  ~RIFTerm() throw();

  static int cmp(const RIFTerm &t1, const RIFTerm &t2) throw();
  static bool cmplt0(const RIFTerm &t1, const RIFTerm &t2) throw();
  static bool cmpeq0(const RIFTerm &t1, const RIFTerm &t2) throw();
  static RIFTerm parse(DPtr<uint8_t> *utf8str)
      throw(BadAllocException, SizeUnknownException,
            InvalidCodepointException, InvalidEncodingException,
            TraceableException, MalformedIRIRefException);

  DPtr<uint8_t> *toUTF8String() const throw(BadAllocException);
  bool equals(const RIFTerm &rhs) const throw();
  RIFTerm &normalize() throw(BadAllocException);
  enum RIFTermType getType() const throw();
  bool isSimple() const throw();
  bool isGround() const throw();
  void getVars(VarSet &vars) const throw();

  // VARIABLE
  RIFVar getVar() const throw(BaseException<enum RIFTermType>);

  // CONSTANT
  RIFConst getConst() const throw(BaseException<enum RIFTermType>);

  // LIST
  DPtr<RIFTerm> *getItems() const throw(BaseException<enum RIFTermType>);

  // FUNCTION
  RIFConst getPred() const throw(BaseException<enum RIFTermType>);
  DPtr<RIFTerm> *getArgs() const throw(BaseException<enum RIFTermType>);

  RIFTerm &operator=(const RIFTerm &rhs) throw(BadAllocException);
};

}

#include "rif/RIFTerm-inl.h"

#endif /* __RIF__RIFTERM_H__ */
