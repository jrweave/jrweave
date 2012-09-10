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

#include "ptr/APtr.h"
#include "ptr/MPtr.h"

namespace rif {

using namespace ptr;
using namespace std;

uint64_t RIFDictionary::encode(const enum RIFAtomicType type) const
    throw(BaseException<enum RIFAtomicType>) {
  if (type == EXTERNAL) {
    THROW(BaseException<enum RIFAtomicType>, EXTERNAL,
          "Cannot encode EXTERNAL.");
  }
  return type == ATOM ? UINT64_C(1) : (uint64_t) type;
}

uint64_t RIFDictionary::encode(const RIFTerm &term)
    throw(BaseException<enum RIFTermType>) {
  if (term.getType() == VARIABLE) {
    return  UINT64_C(0);
  }
  if (term.getType() == FUNCTION) {
    THROW(BaseException<enum RIFTermType>, term.getType(),
          "Cannot encode FUNCTION.");
  }
  Term2IDMap::const_iterator it = this->term2id.find(term);
  if (it != this->term2id.end()) {
    return it->second;
  }
  uint64_t newid = RESERVED_IDS + this->id2term.size();
  this->id2term.insert(pair<uint64_t, RIFTerm>(newid, term));
  this->term2id.insert(pair<RIFTerm, uint64_t>(term, newid));
  return newid;
}

DPtr<uint64_t> *RIFDictionary::encode(const RIFAtomic &atomic)
    throw(BadAllocException, BaseException<enum RIFAtomicType>,
          BaseException<enum RIFTermType>) {
  DPtr<uint64_t> *ids = NULL;
  enum RIFAtomicType type = atomic.getType();
  switch (type) {
  case ATOM: {
    RIFConst pred = atomic.getPred();
    RIFTerm term(pred);
    DPtr<RIFTerm> *args = atomic.getArgs();
    size_t len = 2 + args->size();
    try {
      NEW(ids, MPtr<uint64_t>, len);
    } RETHROW_BAD_ALLOC
    uint64_t *id = ids->dptr();
    *id = this->encode(type);
    *(++id) = this->encode(term);
    RIFTerm *begin = args->dptr();
    RIFTerm *end = begin + args->size();
    for (; begin != end; ++begin) {
      try {
        *(++id) = this->encode(*begin);
      } catch (BaseException<enum RIFTermType> &e) {
        args->drop();
        ids->drop();
        RETHROW(e, "Invalid term found in ATOM.");
      }
    }
    args->drop();
    return ids;
  }
  case EXTERNAL:
    THROW(BaseException<enum RIFAtomicType>, type, "Cannot encode EXTERNAL.");
  case EQUALITY: {
    pair<RIFTerm, RIFTerm> eq = atomic.getEquality();
    try {
      NEW(ids, MPtr<uint64_t>, 3);
    } RETHROW_BAD_ALLOC
    uint64_t *id = ids->dptr();
    *id = this->encode(type);
    try {
      *(++id) = this->encode(eq.first);
      *(++id) = this->encode(eq.second);
    } catch (BaseException<enum RIFTermType> &e) {
      ids->drop();
      RETHROW(e, "Invalid term found in EQUALITY.");
    }
    return ids;
  }
  case MEMBERSHIP: {
    RIFTerm object = atomic.getObject();
    RIFTerm cls = atomic.getClass();
    try {
      NEW(ids, MPtr<uint64_t>, 3);
    } RETHROW_BAD_ALLOC
    uint64_t *id = ids->dptr();
    *id = this->encode(type);
    try {
      *(++id) = this->encode(object);
      *(++id) = this->encode(cls);
    } catch (BaseException<enum RIFTermType> &e) {
      ids->drop();
      RETHROW(e, "Invalid term found in MEMBERSHIP.");
    }
    return ids;
  }
  case SUBCLASS: {
    RIFTerm subclass = atomic.getSubclass();
    RIFTerm superclass = atomic.getSuperclass();
    try {
      NEW(ids, MPtr<uint64_t>, 3);
    } RETHROW_BAD_ALLOC
    uint64_t *id = ids->dptr();
    *id = this->encode(type);
    try {
      *(++id) = this->encode(subclass);
      *(++id) = this->encode(superclass);
    } catch (BaseException<enum RIFTermType> &e) {
      ids->drop();
      RETHROW(e, "Invalid term found in SUBCLASS.");
    }
    return ids;
  }
  case FRAME: {
    vector<RIFTerm> terms;
    RIFTerm object = atomic.getObject();
    TermSet attrs(RIFTerm::cmplt0);
    atomic.getAttrs(attrs);
    TermSet::const_iterator it = attrs.begin();
    for (; it != attrs.end(); ++it) {
      TermSet vals(RIFTerm::cmplt0);
      atomic.getValues(*it, vals);
      TermSet::const_iterator it2 = vals.begin();
      for (; it2 != vals.end(); ++it2) {
        terms.push_back(*it);
        terms.push_back(*it2);
      }
    }
    try {
      NEW(ids, MPtr<uint64_t>, 2*terms.size());
    } RETHROW_BAD_ALLOC
    uint64_t type_id = this->encode(type);
    uint64_t obj_id = this->encode(object);
    uint64_t *id = ids->dptr();
    vector<RIFTerm>::const_iterator term = terms.begin();
    for (; term != terms.end(); term += 2) {
      try {
        *id = type_id;
        *(++id) = obj_id;
        *(++id) = this->encode(*term);
        *(++id) = this->encode(*(term + 1));
        ++id;
      } catch (BaseException<enum RIFTermType> &e) {
        ids->drop();
        RETHROW(e, "Invalid term found in FRAME.");
      }
    }
    return ids;
  }
  }
  THROW(BaseException<enum RIFAtomicType>, type,
        "Unhandle type.  This should never happen.");
}

enum RIFAtomicType RIFDictionary::decodeType(const uint64_t id) const
    throw(BaseException<uint64_t>) {
  if (id == 0) {
    THROW(BaseException<uint64_t>, id, "Cannot decode UNBOUND.");
  }
  if (id >= RESERVED_IDS) {
    THROW(BaseException<uint64_t>, id,
          "Cannot decode TERM.  Call decodeTerm(id) instead.");
  }
  int switch_id = (int) id;
  switch (switch_id) {
  case 1: return ATOM;
  case 2: return EQUALITY;
  case 3: return MEMBERSHIP;
  case 4: return SUBCLASS;
  case 5: return FRAME;
  }
  THROW(BaseException<uint64_t>, id,
        "Unhandled type ID.  This should never happen.");
}

RIFTerm RIFDictionary::decodeTerm(const uint64_t id) const
    throw(BaseException<uint64_t>) {
  if (id == 0) {
    THROW(BaseException<uint64_t>, id, "Cannot decode UNBOUND.");
  }
  if (id < RESERVED_IDS) {
    THROW(BaseException<uint64_t>, id,
          "Cannot decode ATOMIC_TYPE.  Call decodeType(id) instead.");
  }
  ID2TermMap::const_iterator it = this->id2term.find(id);
  if (it == this->id2term.end()) {
    THROW(BaseException<uint64_t>, id, "No encoding for given ID.");
  }
  return it->second;
}

RIFAtomic RIFDictionary::decode(DPtr<uint64_t> *ids, map<size_t, RIFVar> *vars)
    const throw(BaseException<void*>, SizeUnknownException,
                BaseException<uint64_t>, TraceableException) {
  if (ids == NULL) {
    THROW(BaseException<void*>, NULL, "ids must not be NULL.");
  }
  if (!ids->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (ids->size() <= 1) {
    THROW(TraceableException, "Invalid empty or singleton ids.");
  }
  const uint64_t *id = ids->dptr();
  const uint64_t *end = id + ids->size();
  enum RIFAtomicType type;
  try {
    type = this->decodeType(*id);
  } JUST_RETHROW(BaseException<uint64_t>, "(rethrow)")
  vector<RIFTerm> terms;
  terms.reserve(ids->size() - 1);
  for (++id; id != end; ++id) {
    if (type == FRAME && (id - ids->dptr()) % 4 == 0) {
      if (this->getIDType(*id) != ATOMIC_TYPE ||
          this->decodeType(*id) != FRAME) {
        THROW(BaseException<uint64_t>, *id, "Invalid encoded FRAME.");
      }
    } else if (*id == 0) {
      if (vars == NULL) {
        THROW(BaseException<uint64_t>, *id,
              "No variable given for unbound id.");
      }
      size_t pos = id - ids->dptr();
      map<size_t, RIFVar>::const_iterator var = vars->find(pos);
      if (var == vars->end()) {
        THROW(BaseException<uint64_t>, *id,
              "No variable given for unbound id.");
      }
      RIFTerm varterm(var->second);
      terms.push_back(varterm);
    } else {
      try {
        terms.push_back(this->decodeTerm(*id));
      } JUST_RETHROW(BaseException<uint64_t>, "(rethrow)")
    }
  }
  switch (type) {
  case ATOM: {
    vector<RIFTerm>::const_iterator mark = terms.begin();
    RIFTerm pred = *mark;
    if (pred.getType() != CONSTANT) {
      THROW(TraceableException, "Invalid ATOM encoding.");
    }
    DPtr<RIFTerm> *args;
    try {
      NEW(args, APtr<RIFTerm>, terms.size() - 1);
    } RETHROW_BAD_ALLOC
    RIFTerm *arg = args->dptr();
    for (++mark; mark != terms.end(); ++mark) {
      try {
        *arg = *mark;
      } catch (BadAllocException &e) {
        args->drop();
        RETHROW(e, "(rethrow)");
      }
      ++arg;
    }
    try {
      RIFConst cnst = pred.getConst();
      RIFAtomic atomic(cnst, args, false);
      args->drop();
      return atomic;
    } catch (BadAllocException &e) {
      args->drop();
      RETHROW(e, "(rethrow)");
    }
  }
  case EQUALITY:
  case MEMBERSHIP:
  case SUBCLASS: {
    if (terms.size() != 2) {
      THROW(TraceableException,
            "Invalid EQUALITY, MEMBERSHIP, or SUBCLASS encoding.");
    }
    try {
      return RIFAtomic(type, terms[0], terms[1]);
    } JUST_RETHROW(BadAllocException, "(rethrow)")
  }
  case FRAME: {
    if (terms.size() % 3 != 0) {
      THROW(TraceableException, "Invalid FRAME encoding.");
    }
    SlotMap slots(RIFTerm::cmplt0);
    vector<RIFTerm>::const_iterator it = terms.begin();
    for (; it != terms.end(); it += 3) {
      if (it != terms.begin() && !terms[0].equals(*it)) {
        THROW(TraceableException, "Invalid FRAME encoding.");
      }
      slots.insert(pair<RIFTerm, RIFTerm>(*(it+1), *(it+2)));
    }
    try {
      return RIFAtomic(terms[0], slots);
    } JUST_RETHROW(BadAllocException, "(rethrow)")
  }
  }
  THROW(TraceableException, "Unhandled case; this should never happen.");
}

RIFDictionary &RIFDictionary::operator=(const RIFDictionary &rhs) throw() {
  this->term2id = rhs.term2id;
  this->id2term = rhs.id2term;
  return *this;
}

}
