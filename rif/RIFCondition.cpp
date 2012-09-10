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

#include "ptr/APtr.h"

namespace rif {

using namespace ptr;
using namespace std;

RIFCondition::RIFCondition(const RIFAtomic &atomic) throw(BadAllocException)
    : type(ATOMIC) {
  RIFAtomic *a;
  try {
    NEW(a, RIFAtomic, atomic);
  } RETHROW_BAD_ALLOC
  this->state = (void*) a;
}

RIFCondition::RIFCondition(const enum RIFCondType type,
    DPtr<RIFCondition> *subs)
    throw(SizeUnknownException, BaseException<enum RIFCondType>)
    : type(type), state((void*)subs) {
  if (type != CONJUNCTION && type != DISJUNCTION) {
    THROW(BaseException<enum RIFCondType>, type,
          "type must be CONJUNCTION or DISJUNCTION.");
  }
  if (subs != NULL) {
    if (!subs->sizeKnown()) {
      THROWX(SizeUnknownException);
    }
    subs->hold();
  }
}

RIFCondition::RIFCondition(const RIFCondition &copy, bool dont_negate)
    throw(BadAllocException)
    : type(dont_negate ? copy.type : NEGATION) {
  if (dont_negate) {
    switch (copy.type) {
    case ATOMIC: {
      RIFAtomic *a;
      try {
        NEW(a, RIFAtomic, *((RIFAtomic*)copy.state));
      } RETHROW_BAD_ALLOC
      this->state = (void*) a;
      break;
    }
    case CONJUNCTION:
    case DISJUNCTION: {
      DPtr<RIFCondition> *p = (DPtr<RIFCondition>*) copy.state;
      if (p != NULL) {
        p->hold();
      }
      this->state = (void*) p;
      break;
    }
    case NEGATION: {
      RIFCondition *c;
      try {
        NEW(c, RIFCondition, *((RIFCondition*)copy.state));
      } RETHROW_BAD_ALLOC
      this->state = (void*) c;
      break;
    }
    case EXISTENTIAL: {
      exist_state *es;
      try {
        NEW(es, exist_state, *((exist_state*)copy.state));
      } RETHROW_BAD_ALLOC
      this->state = (void*) es;
      break;
    }
    }
  } else {
    RIFCondition *c;
    try {
      NEW(c, RIFCondition, copy);
    } RETHROW_BAD_ALLOC
    this->state = (void*) c;
  }
}

RIFCondition::RIFCondition(DPtr<RIFVar> *vars, const RIFCondition &sub)
    throw(BaseException<void*>, SizeUnknownException, TraceableException)
    : type(EXISTENTIAL) {
  exist_state *es;
  try {
    NEW(es, exist_state, vars, sub);
  } JUST_RETHROW(BaseException<void*>, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(TraceableException, "(rethrow)")
  this->state = (void*) es;
}

RIFCondition::RIFCondition(const RIFCondition &copy) throw(BadAllocException)
    : type(copy.type) {
  switch (copy.type) {
  case ATOMIC: {
    RIFAtomic *a;
    try {
      NEW(a, RIFAtomic, *((RIFAtomic*)copy.state));
    } RETHROW_BAD_ALLOC
    this->state = (void*) a;
    break;
  }
  case CONJUNCTION:
  case DISJUNCTION: {
    DPtr<RIFCondition> *p = (DPtr<RIFCondition>*) copy.state;
    if (p != NULL) {
      p->hold();
    }
    this->state = (void*) p;
    break;
  }
  case NEGATION: {
    RIFCondition *c;
    try {
      NEW(c, RIFCondition, *((RIFCondition*)copy.state));
    } RETHROW_BAD_ALLOC
    this->state = (void*) c;
    break;
  }
  case EXISTENTIAL: {
    exist_state *es;
    try {
      NEW(es, exist_state, *((exist_state*)copy.state));
    } RETHROW_BAD_ALLOC
    this->state = (void*) es;
    break;
  }
  }
}

RIFCondition::~RIFCondition() throw() {
  switch (this->type) {
  case ATOMIC:
    DELETE((RIFAtomic*)this->state);
    break;
  case CONJUNCTION:
  case DISJUNCTION:
    if (this->state != NULL) {
      ((DPtr<RIFCondition>*)this->state)->drop();
    }
    break;
  case NEGATION:
    DELETE((RIFCondition*)this->state);
    break;
  case EXISTENTIAL:
    DELETE((exist_state*)this->state);
    break;
  }
}

// EXISTENTIALs that are logically the same but have different variable names
// compare as different.  That's because this class represents a PURELY
// SYNTACTIC ENTITY.
int RIFCondition::cmp(const RIFCondition &c1, const RIFCondition &c2) throw() {
  if (c1.type != c2.type) {
    return ((int)c1.type) - ((int)c2.type);
  }
  switch (c1.type) {
  case ATOMIC:
    return RIFAtomic::cmp(*((RIFAtomic*)c1.state), *((RIFAtomic*)c2.state));
  case CONJUNCTION:
  case DISJUNCTION: {
    DPtr<RIFCondition> *p1 = (DPtr<RIFCondition> *) c1.state;
    DPtr<RIFCondition> *p2 = (DPtr<RIFCondition> *) c2.state;
    if (p1 == NULL || p1->size() <= 0) {
      return (p2 == NULL || p2->size() <= 0) ? 0 : -1;
    }
    if (p2 == NULL || p2->size() <= 0) {
      return 1;
    }
    if (p1->size() != p2->size()) {
      return p1->size() < p2->size() ? -1 : 1;
    }
    RIFCondition *begin = p1->dptr();
    RIFCondition *end = begin + p1->size();
    RIFCondition *mark = p2->dptr();
    for (; begin != end; ++begin) {
      int c = RIFCondition::cmp(*begin, *mark);
      if (c != 0) {
        return c;
      }
      ++mark;
    }
    return 0;
  }
  case NEGATION:
    return RIFCondition::cmp(*((RIFCondition*)c1.state),
                             *((RIFCondition*)c2.state));
  case EXISTENTIAL: {
    exist_state *e1 = (exist_state*) c1.state;
    exist_state *e2 = (exist_state*) c2.state;
    if (e1->vars->size() != e2->vars->size()) {
      return e1->vars->size() < e2->vars->size() ? -1 : 1;
    }
    RIFVar *begin = e1->vars->dptr();
    RIFVar *end = begin + e1->vars->size();
    RIFVar *mark = e2->vars->dptr();
    for (; begin != end; ++begin) {
      int c = RIFVar::cmp(*begin, *mark);
      if (c != 0) {
        return c;
      }
      ++mark;
    }
    return RIFCondition::cmp(e1->sub, e2->sub);
  }
  }
}

RIFCondition RIFCondition::parse(DPtr<uint8_t> *utf8str)
    throw(BadAllocException, SizeUnknownException,
          InvalidCodepointException, InvalidEncodingException,
          MalformedIRIRefException, TraceableException) {
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  const uint8_t *mark = begin;
  for (; mark != end && *mark != to_ascii('('); ++mark) {
    // find first '(', or end
  }
  enum RIFCondType type = ATOMIC;
  if (mark != end) {
    const uint8_t *backup = mark - 1;
    for (; backup != begin && is_space(*backup); --backup) {
      // find non-space char preceding '('
    }
    ++backup;
    size_t len = backup - begin;
    if (len == 2 && ascii_strncmp(begin, "Or", 2) == 0) {
      type = DISJUNCTION;
    } else if (len == 3 && ascii_strncmp(begin, "And", 3) == 0) {
      type = CONJUNCTION;
    } else if (len == 3 && ascii_strncmp(begin, "Not", 3) == 0) {
      type = NEGATION;
    } else if (len == 4 && ascii_strncmp(begin, "INeg", 4) == 0) {
      type = NEGATION;
    } else if (len >= 6 && ascii_strncmp(begin, "Exists", 6) == 0) {
      type = EXISTENTIAL;
    }
  }
  if (type == ATOMIC) {
    try {
      RIFAtomic atomic = RIFAtomic::parse(utf8str);
      return RIFCondition(atomic);
    } JUST_RETHROW(BadAllocException, "(rethrow)")
      JUST_RETHROW(SizeUnknownException, "(rethrow)")
      JUST_RETHROW(InvalidCodepointException, "(rethrow)")
      JUST_RETHROW(InvalidEncodingException, "(rethrow)")
      JUST_RETHROW(MalformedIRIRefException, "(rethrow)")
      JUST_RETHROW(TraceableException, "(rethrow)")
  }
  DPtr<RIFVar> *vars = NULL;
  if (type == EXISTENTIAL) {
    vector<RIFVar> vec;
    mark = begin + 6;
    for (; mark != end && is_space(*mark); ++mark) {
      // skip whitespace
    }
    const uint8_t *next = mark;
    while (mark != end && *mark != to_ascii('(')) {
      for (; next != end && !is_space(*next); ++next) {
        // find end of variable
      }
      DPtr<uint8_t> *varstr = utf8str->sub(mark - begin, next - mark);
      RIFVar var;
      try {
        var = RIFVar::parse(varstr);
        varstr->drop();
        vec.push_back(var);
        for (; next != end && is_space(*next); ++next) {
          // skip whitespace
        }
        mark = next;
      } catch (BadAllocException &e) {
        varstr->drop();
        RETHROW(e, "(rethrow)");
      } catch (InvalidCodepointException &e) {
        varstr->drop();
        RETHROW(e, "(rethrow)");
      } catch (InvalidEncodingException &e) {
        varstr->drop();
        RETHROW(e, "(rethrow)");
      } catch (TraceableException &e) {
        varstr->drop();
        for (; next != end && is_space(*next); ++next) {
          // skip whitespace
        }
        // ignore and try again
      }
    }
    try {
      NEW(vars, APtr<RIFVar>, vec.size());
    } RETHROW_BAD_ALLOC
    vector<RIFVar>::const_iterator it = vec.begin();
    RIFVar *varp = vars->dptr();
    for (; it != vec.end(); ++it) {
      *varp = *it;
      ++varp;
    }
  }
  for (++mark; mark != end && is_space(*mark); ++mark) {
    // find beginning of subformula(s)
  }
  if (mark == end) {
    if (vars != NULL) {
      vars->drop();
    }
    THROW(TraceableException,
          "No closing ')' on non-atomic condition formula.");
  }
  for (--end; end != mark && is_space(*end); --end) {
    // find closing ')'
  }
  if (*end != to_ascii(')')) {
    THROW(TraceableException,
          "No closing ')' on non-atomic condition formula.");
  }
  if (end != mark) {
    for (--end; end != mark && is_space(*end); --end) {
      // find end of subformula(s)
    }
    ++end;
  }
  if (type == NEGATION || type == EXISTENTIAL) {
    DPtr<uint8_t> *sub = utf8str->sub(mark - begin, end - mark);
    try {
      RIFCondition cond = RIFCondition::parse(sub);
      sub->drop();
      if (type == NEGATION) {
        return RIFCondition(cond, false);
      }
      RIFCondition ret(vars, cond);
      vars->drop();
      return ret;
    } catch (BadAllocException &e) {
      if (vars != NULL) {
        vars->drop();
      }
      sub->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidCodepointException &e) {
      if (vars != NULL) {
        vars->drop();
      }
      sub->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidEncodingException &e) {
      if (vars != NULL) {
        vars->drop();
      }
      sub->drop();
      RETHROW(e, "(rethrow)");
    } catch (MalformedIRIRefException &e) {
      if (vars != NULL) {
        vars->drop();
      }
      sub->drop();
      RETHROW(e, "(rethrow)");
    } catch (TraceableException &e) {
      if (vars != NULL) {
        vars->drop();
      }
      sub->drop();
      RETHROW(e, "(rethrow)");
    }
  }
  vector<RIFCondition> conds;
  begin = mark;
  while (mark != end) {
    for (; mark != end && !is_space(*mark); ++mark) {
      // find possible end of subformula
    }
    DPtr<uint8_t> *sub = utf8str->sub(begin - utf8str->dptr(), mark - begin);
    try {
      RIFCondition cond = RIFCondition::parse(sub);
      sub->drop();
      conds.push_back(cond);
      for (; mark != end && is_space(*mark); ++mark) {
        // find next non-space char in formula, or end
      }
      begin = mark;
    } catch (BadAllocException &e) {
      sub->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidCodepointException &e) {
      sub->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidEncodingException &e) {
      sub->drop();
      RETHROW(e, "(rethrow)");
    } catch (MalformedIRIRefException &e) {
      sub->drop();
      for (; mark != end && is_space(*mark); ++mark) {
        // find next non-space char in formula, or end
      }
      // ignore and try again
    } catch (TraceableException &e) {
      sub->drop();
      for (; mark != end && is_space(*mark); ++mark) {
        // find next non-space char in formula, or end
      }
      // ignore and try again
    }
  }
  if (begin != end) {
    THROW(TraceableException,
          "Unrecognized subformula in CONJUNCTION or DISJUNCTION.");
  }
  DPtr<RIFCondition> *subs;
  try {
    NEW(subs, APtr<RIFCondition>, conds.size());
  } RETHROW_BAD_ALLOC
  vector<RIFCondition>::const_iterator it = conds.begin();
  RIFCondition *condp = subs->dptr();
  for (; it != conds.end(); ++it) {
    *condp = *it;
    ++condp;
  }
  try {
    RIFCondition ret(type, subs);
    subs->drop();
    return ret;
  } catch (BadAllocException &e) {
    subs->drop();
    RETHROW(e, "(rethrow)");
  } catch (InvalidCodepointException &e) {
    subs->drop();
    RETHROW(e, "(rethrow)");
  } catch (InvalidEncodingException &e) {
    subs->drop();
    RETHROW(e, "(rethrow)");
  } catch (MalformedIRIRefException &e) {
    subs->drop();
    RETHROW(e, "(rethrow)");
  } catch (TraceableException &e) {
    subs->drop();
    RETHROW(e, "(rethrow)");
  }
}

DPtr<uint8_t> *RIFCondition::toUTF8String() const THROWS(BadAllocException) {
  vector<void*> subs;
  DPtr<uint8_t> *str = NULL;
  size_t len = 0;
  switch (this->type) {
  case ATOMIC: {
    str = ((RIFAtomic*)this->state)->toUTF8String();
    len += str->size();
    subs.push_back((void*) str);
    break;
  }
  case CONJUNCTION:
    ++len;
    // fall-through
  case DISJUNCTION: {
    len += 4;
    if (this->state != NULL) {
      DPtr<RIFCondition> *conds = (DPtr<RIFCondition>*) this->state;
      if (conds->size() > 1) {
        len += conds->size() - 1;
      }
      subs.reserve(conds->size());
      RIFCondition *begin = conds->dptr();
      RIFCondition *end = begin + conds->size();
      for (; begin != end; ++begin) {
        try {
          str = begin->toUTF8String();
        } catch (BadAllocException &e) {
          vector<void*>::iterator it = subs.begin();
          for (; it != subs.end(); ++it) {
            ((DPtr<uint8_t>*)(*it))->drop();
          }
          RETHROW(e, "(rethrow)");
        }
        len += str->size();
        subs.push_back((void*)str);
      }
    }
    break;
  }
  case NEGATION: {
    len += 5;
    str = ((RIFCondition*)this->state)->toUTF8String();
    len += str->size();
    subs.push_back((void*) str);
    break;
  }
  case EXISTENTIAL: {
    len += 9;
    exist_state *e = (exist_state*) this->state;
    len += e->vars->size();
    RIFVar *begin = e->vars->dptr();
    RIFVar *end = begin + e->vars->size();
    for (; begin != end; ++begin) {
      try {
        str = begin->toUTF8String();
      } catch (BadAllocException &e) {
        vector<void*>::iterator it = subs.begin();
        for (; it != subs.end(); ++it) {
          ((DPtr<uint8_t>*)(*it))->drop();
        }
        RETHROW(e, "(rethrow)");
      }
      len += str->size();
      subs.push_back((void*)str);
    }
    try {
      str = e->sub.toUTF8String();
    } catch (BadAllocException &e) {
      vector<void*>::iterator it = subs.begin();
      for (; it != subs.end(); ++it) {
        ((DPtr<uint8_t>*)(*it))->drop();
      }
      RETHROW(e, "(rethrow)");
    }
    len += str->size();
    subs.push_back((void*)str);
    break;
  }
  }
  try {
    NEW(str, MPtr<uint8_t>, len);
  } catch (BadAllocException &e) {
    vector<void*>::iterator it = subs.begin();
    for (; it != subs.end(); ++it) {
      ((DPtr<uint8_t>*)(*it))->drop();
    }
    RETHROW(e, "(rethrow)");
  }
  uint8_t *p = str->dptr();
  switch (this->type) {
  case CONJUNCTION:
    ascii_strcpy(p, "And(");
    p += 4;
    break;
  case DISJUNCTION:
    ascii_strcpy(p, "Or(");
    p += 3;
    break;
  case NEGATION:
    ascii_strcpy(p, "Not(");
    p += 4;
    break;
  case EXISTENTIAL:
    ascii_strcpy(p, "Exists ");
    p += 7;
    break;
  }
  if (subs.size() > 0) {
    vector<void*>::iterator it = subs.begin();
    DPtr<uint8_t> *sub = NULL;
    for (; it + 1 != subs.end(); ++it) {
      DPtr<uint8_t> *sub = (DPtr<uint8_t>*)(*it);
      memcpy(p, sub->dptr(), sub->size() * sizeof(uint8_t));
      p += sub->size();
      sub->drop();
      *p = to_ascii(' ');
      ++p;
    }
    if (this->type == EXISTENTIAL) {
      *p = to_ascii('(');
      ++p;
    }
    sub = (DPtr<uint8_t>*)(*it);
    memcpy(p, sub->dptr(), sub->size() * sizeof(uint8_t));
    p += sub->size();
    sub->drop();
  }
  if (this->type != ATOMIC) {
    *p = to_ascii(')');
  }
  return str;
}
TRACE(BadAllocException, "(trace)")

RIFCondition &RIFCondition::normalize() THROWS(BadAllocException) {
  // TODO actually normalize the formula
  // difficulty is with bringing out existential quantifiers
  switch (this->type) {
  case ATOMIC:
    ((RIFAtomic*)this->state)->normalize();
    return *this;
  case CONJUNCTION:
  case DISJUNCTION:
    if (this->state != NULL) {
      DPtr<RIFCondition> *subs = (DPtr<RIFCondition>*) this->state;
      RIFCondition *mark = subs->dptr();
      RIFCondition *end = mark + subs->size();
      for (; mark != end; ++mark) {
        mark->normalize();
      }
    }
    return *this;
  case NEGATION:
    ((RIFCondition*)this->state)->normalize();
    return *this;
  case EXISTENTIAL: {
    exist_state *e = (exist_state*) this->state;
    RIFVar *mark = e->vars->dptr();
    RIFVar *end = mark + e->vars->size();
    for (; mark != end; ++mark) {
      mark->normalize();
    }
    e->sub.normalize();
    return *this;
  }
  }
}
TRACE(BadAllocException, "(trace)")

void RIFCondition::getVars(VarSet &vars) const throw() {
  switch (this->type) {
  case ATOMIC:
    ((RIFAtomic*)this->state)->getVars(vars);
    return;
  case CONJUNCTION:
  case DISJUNCTION:
    if (this->state != NULL) {
      DPtr<RIFCondition> *subs = (DPtr<RIFCondition>*) this->state;
      RIFCondition *mark = subs->dptr();
      RIFCondition *end = mark + subs->size();
      for (; mark != end; ++mark) {
        mark->getVars(vars);
      }
    }
    return;
  case NEGATION:
    ((RIFCondition*)this->state)->getVars(vars);
    return;
  case EXISTENTIAL: {
    VarSet local_vars(vars.key_comp());
    exist_state *e = (exist_state*) this->state;
    e->sub.getVars(local_vars);
    RIFVar *mark = e->vars->dptr();
    RIFVar *end = mark + e->vars->size();
    for (; mark != end; ++mark) {
      local_vars.erase(*mark);
    }
    vars.insert(local_vars.begin(), local_vars.end());
    return;
  }
  }
}

RIFAtomic RIFCondition::getAtomic() const
    throw(BaseException<enum RIFCondType>) {
  if (this->type != ATOMIC) {
    THROW(BaseException<enum RIFCondType>, this->type,
          "Type must be ATOMIC to call getAtomic().");
  }
  return *((RIFAtomic*)this->state);
}

DPtr<RIFCondition> *RIFCondition::getSubformulas() const
    throw(BaseException<enum RIFCondType>) {
  if (this->type != CONJUNCTION && this->type != DISJUNCTION) {
    THROW(BaseException<enum RIFCondType>, this->type,
          "Type must be CONJUNCTION or DISJUNCTION to call getSubformulas().");
  }
  if (this->state == NULL) {
    DPtr<RIFCondition> *p = NULL;
    NEW(p, APtr<RIFCondition>, (size_t)0);
    return p;
  }
  DPtr<RIFCondition> *conds = (DPtr<RIFCondition> *) this->state;
  conds->hold();
  return conds;
}

RIFCondition RIFCondition::getSubformula() const
    throw(BaseException<enum RIFCondType>) {
  if (this->type != NEGATION && this->type != EXISTENTIAL) {
    THROW(BaseException<enum RIFCondType>, this->type,
          "Type must be NEGATION or EXISTENTIAL to call getSubformula().");
  }
  return *((RIFCondition*)this->state);
}

RIFCondition &RIFCondition::operator=(const RIFCondition &rhs)
    THROWS(BadAllocException) {
  if (this == &rhs) {
    return *this;
  }
  if (this->type == rhs.type) {
    switch (this->type) {
    case ATOMIC:
      *((RIFAtomic*)this->state) = *((RIFAtomic*)rhs.state);
      return *this;
    case NEGATION:
      *((RIFCondition*)this->state) = *((RIFCondition*)rhs.state);
      return *this;
    case EXISTENTIAL:
      *((exist_state*)this->state) = *((exist_state*)rhs.state);
      return *this;
    }
  }
  void *new_state = NULL;
  switch (rhs.type) {
  case ATOMIC: {
    RIFAtomic *a;
    try {
      NEW(a, RIFAtomic, *((RIFAtomic*)rhs.state));
    } RETHROW_BAD_ALLOC
    new_state = (void*) a;
    break;
  }
  case CONJUNCTION:
  case DISJUNCTION: {
    if (this->state != NULL) {
      DPtr<RIFCondition> *p = (DPtr<RIFCondition>*) rhs.state;
      p->hold();
      new_state = (void*) p;
    }
    break;
  }
  case NEGATION: {
    RIFCondition *c;
    try {
      NEW(c, RIFCondition, *((RIFCondition*)rhs.state));
    } RETHROW_BAD_ALLOC
    new_state = (void*) c;
    break;
  }
  case EXISTENTIAL: {
    exist_state *e;
    try {
      NEW(e, exist_state, *((exist_state*)rhs.state));
    } RETHROW_BAD_ALLOC
    new_state = (void*) e;
    break;
  }
  }
  switch (this->type) {
  case ATOMIC:
    DELETE((RIFAtomic*)this->state);
    break;
  case CONJUNCTION:
  case DISJUNCTION:
    if (this->state != NULL) {
      ((DPtr<RIFCondition>*)this->state)->drop();
    }
    break;
  case NEGATION:
    DELETE((RIFCondition*)this->state);
    break;
  case EXISTENTIAL:
    DELETE((exist_state*)this->state);
    break;
  }
  this->state = new_state;
  this->type = rhs.type;
  return *this;
}
TRACE(BadAllocException, "(trace)")

}
