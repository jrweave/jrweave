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

#include "ptr/APtr.h"
#include "ptr/MPtr.h"

namespace rif {

using namespace ptr;
using namespace rif;
using namespace std;

RIFTerm::RIFTerm(const RIFConst &c) throw(BadAllocException)
    : type(CONSTANT) {
  RIFConst *x;
  try {
    NEW(x, RIFConst, c);
  } catch(bad_alloc &ba) {
    THROW(BadAllocException, sizeof(RIFConst));
  }
  this->state = x;
}

RIFTerm::RIFTerm(const RIFVar &v) throw(BadAllocException)
    : type(VARIABLE) {
  RIFVar *x;
  try {
    NEW(x, RIFVar, v);
  } catch(bad_alloc &ba) {
    THROW(BadAllocException, sizeof(RIFVar));
  }
  this->state = x;
}

RIFTerm::RIFTerm(DPtr<RIFTerm> *list)
    throw(SizeUnknownException, TraceableException)
    : type(LIST) {
  if (list != NULL) {
    if (!list->sizeKnown()) {
      THROWX(SizeUnknownException);
    }
    RIFTerm *begin = list->dptr();
    RIFTerm *end = begin + list->size();
    for (; begin != end; ++begin) {
      if (!begin->isGround()) {
        THROW(TraceableException, "List terms must be ground.");
      }
    }
    list->hold();
  }
  this->state = list;
}

RIFTerm::RIFTerm(const RIFConst &pred, DPtr<RIFTerm> *args)
    throw(SizeUnknownException, BadAllocException)
    : type(FUNCTION) {
  func_state *f;
  try {
    NEW(f, func_state, pred, args);
  } catch (bad_alloc &ba) {
    THROW(BadAllocException, sizeof(func_state));
  } JUST_RETHROW(SizeUnknownException, "(rethrow)")
  this->state = f;
}

RIFTerm::RIFTerm(const RIFTerm &copy) throw(BadAllocException)
    : type(copy.type) {
  switch(copy.type) {
  case CONSTANT: {
    RIFConst *c;
    try {
      NEW(c, RIFConst, *((RIFConst*) copy.state));
    } RETHROW_BAD_ALLOC
    this->state = c;
    break;
  }
  case VARIABLE: {
    RIFVar *v;
    try {
      NEW(v, RIFVar, *((RIFVar*) copy.state));
    } RETHROW_BAD_ALLOC
    this->state = v;
    break;
  }
  case LIST: {
    DPtr<RIFTerm> *l = (DPtr<RIFTerm>*) copy.state;
    if (l != NULL) {
      l->hold();
    }
    this->state = l;
    break;
  }
  case FUNCTION: {
    func_state *f1 = (func_state *) copy.state;
    func_state *f2;
    try {
      NEW(f2, func_state, *f1);
    } RETHROW_BAD_ALLOC
    this->state = f2;
    break;
  }
  }
}

RIFTerm::~RIFTerm() throw() {
  switch(this->type) {
  case VARIABLE: {
    DELETE((RIFVar*) this->state);
    break;
  }
  case CONSTANT: {
    DELETE((RIFConst*) this->state);
    break;
  }
  case LIST: {
    if (this->state != NULL) {
      ((DPtr<RIFTerm> *) this->state)->drop();
    }
    break;
  }
  case FUNCTION: {
    DELETE((func_state*) this->state);
    break;
  }
  }
}

int RIFTerm::cmp(const RIFTerm &t1, const RIFTerm &t2) throw() {
  if (t1.type != t2.type) {
    return ((int) t1.type) - ((int) t2.type);
  }
  switch (t1.type) {
  case VARIABLE:
    return RIFVar::cmp(*((RIFVar*)t1.state), *((RIFVar*)t2.state));
  case CONSTANT:
    return RIFConst::cmp(*((RIFConst*)t1.state), *((RIFConst*)t2.state));
  case LIST: {
    // sort primarily on length, for efficiency
    DPtr<RIFTerm> *args1 = (DPtr<RIFTerm> *)t1.state;
    DPtr<RIFTerm> *args2 = (DPtr<RIFTerm> *)t2.state;
    size_t len1 = args1 == NULL ? 0 : args1->size();
    size_t len2 = args2 == NULL ? 0 : args2->size();
    if (len1 != len2) {
      return len1 < len2 ? -1 : 1;
    }
    if (len1 == 0) {
      return 0;
    }
    RIFTerm *begin = (RIFTerm *)args1->dptr();
    RIFTerm *end = begin + args1->size();
    RIFTerm *mark = (RIFTerm *)args2->dptr();
    for (; begin != end; ++begin) {
      int c = RIFTerm::cmp(*begin, *mark);
      if (c != 0) {
        return c;
      }
      ++mark;
    }
    return 0;
  }
  case FUNCTION: {
    func_state *f1 = (func_state *)t1.state;
    func_state *f2 = (func_state *)t2.state;
    int c = RIFConst::cmp(f1->pred, f2->pred);
    if (c != 0) {
      return c;
    }
    DPtr<RIFTerm> *args1 = f1->args;
    DPtr<RIFTerm> *args2 = f2->args;
    size_t len1 = args1 == NULL ? 0 : args1->size();
    size_t len2 = args2 == NULL ? 0 : args2->size();
    if (len1 != len2) {
      return len1 < len2 ? -1 : 1;
    }
    if (len1 == 0) {
      return 0;
    }
    RIFTerm *begin = (RIFTerm *)args1->dptr();
    RIFTerm *end = begin + args1->size();
    RIFTerm *mark = (RIFTerm *)args2->dptr();
    for (; begin != end; ++begin) {
      int c = RIFTerm::cmp(*begin, *mark);
      if (c != 0) {
        return c;
      }
      ++mark;
    }
    return 0;
  }
  }
}

RIFTerm RIFTerm::parse(DPtr<uint8_t> *utf8str) throw(BadAllocException,
    SizeUnknownException, InvalidCodepointException, InvalidEncodingException,
    TraceableException, MalformedIRIRefException) {
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }

  // Try constant.
  try {
    RIFConst c = RIFConst::parse(utf8str);
    return RIFTerm(c);
  } JUST_RETHROW(BadAllocException, "(rethrow)")
    JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(InvalidEncodingException, "(rethrow)")
    JUST_RETHROW(MalformedIRIRefException, "(rethrow)")
  catch (TraceableException &e) {
    // ignore, try something else
  }

  // Try variable.
  try {
    RIFVar v = RIFVar::parse(utf8str);
    return RIFTerm(v);
  } JUST_RETHROW(BadAllocException, "(rethrow)")
    JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(InvalidEncodingException, "(rethrow)")
  catch (TraceableException &e) {
    // ignore, try something else
  }

  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  const uint8_t *mark;

  // Try list or function.
  for (mark = begin; mark != end && *mark != to_ascii('('); ++mark) {
    // find opening paren
  }
  if (mark == end) {
    THROW(TraceableException, "Expected opening '(' for list or function.");
  }
  const uint8_t *predend = NULL;
  if (mark - begin != 4 || ascii_strncmp(begin, "List", 4) != 0) {
    // parse to to make sure valid for now
    // yes, this is inefficient
    do {
      DPtr<uint8_t> *predstr = utf8str->sub(0, mark - begin);
      try {
        RIFConst::parse(predstr);
        predstr->drop();
        predend = mark;
      } catch (BadAllocException &e) {
        predstr->drop();
        RETHROW(e, "(rethrow)");
      } catch (InvalidCodepointException &e) {
        predstr->drop();
        RETHROW(e, "(rethrow)");
      } catch (InvalidEncodingException &e) {
        predstr->drop();
        RETHROW(e, "(rethrow)");
      } catch (MalformedIRIRefException &e) {
        predstr->drop();
        RETHROW(e, "(rethrow)");
      } catch (TraceableException &e) {
        predstr->drop();
        if (mark != end) {
          for (++mark; mark != end && *mark != to_ascii('('); ++mark) {
            // find next open paren
          }
        }
      }
    } while (mark != end && predend == NULL);
    if (predend == NULL) {
      THROW(TraceableException, "Invalid function.");
    }
  }
  begin = mark;
  for (--end; begin != end && is_space(*end); --end) {
    // find last non-space character
  }
  if (begin == end || *end != to_ascii(')')) {
    THROW(TraceableException,
          "Couldn't find closing ')' for list or function.");
  }
  for (++begin; begin != end && is_space(*begin); ++begin) {
    // find first list item
  }
  if (begin != end) {
    for (--end; begin != end && is_space(*end); --end) {
      // find last character of last list item
    }
    ++end;
  }
  vector<RIFTerm> items;
  mark = begin;
  while (begin != end) {
    if (mark == end) {
      THROW(TraceableException, "Malformed list or function.");
    }
    for (; mark != end && !is_space(*mark); ++mark) {
      // find first space character
    }
    DPtr<uint8_t> *item = utf8str->sub(begin - utf8str->dptr(),
                                       mark - begin);
    try {
      RIFTerm term = RIFTerm::parse(item);
      item->drop();
      items.push_back(term);
      for (begin = mark; begin != end && is_space(*begin); ++begin) {
        // set begin to start of next item, or end
      }
      mark = begin;
    } catch (BadAllocException &e) {
      item->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidCodepointException &e) {
      item->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidEncodingException &e) {
      item->drop();
      RETHROW(e, "(rethrow)");
    } catch (MalformedIRIRefException &e) {
      item->drop();
      for (; mark != end && is_space(*mark); ++mark) {
        // move to next non-space
      }
    } catch (TraceableException &e) {
      item->drop();
      for (; mark != end && is_space(*mark); ++mark) {
        // move to next non-space
      }
    }
  }
  DPtr<RIFTerm> *terms = NULL;
  if (items.size() > 0) {
    try {
      NEW(terms, APtr<RIFTerm>, items.size());
    } RETHROW_BAD_ALLOC
    RIFTerm *termp = terms->dptr();
    vector<RIFTerm>::const_iterator vbegin = items.begin();
    vector<RIFTerm>::const_iterator vend = items.end();
    for (; vbegin != vend; ++vbegin) {
      *termp = *vbegin;
      ++termp;
    }
  }
  try {
    if (predend == NULL) {
      RIFTerm ret(terms);
      if (terms != NULL) {
        terms->drop();
      }
      return ret;
    }
  } catch (TraceableException &e) {
    if (terms != NULL) {
      terms->drop();
    }
    RETHROW(e, "(rethrow)");
  }
  DPtr<uint8_t> *predstr = utf8str->sub(0, predend - utf8str->dptr());
  try {
    RIFConst pred = RIFConst::parse(predstr);
    predstr->drop();
    RIFTerm ret(pred, terms);
    if (terms != NULL) {
      terms->drop();
    }
    return ret;
  } catch (bad_alloc &e) {
    predstr->drop();
    if (terms != NULL) {
      terms->drop();
    }
    THROW(BadAllocException, sizeof(RIFConst));
  } catch (BadAllocException &e) {
    predstr->drop();
    if (terms != NULL) {
      terms->drop();
    }
    RETHROW(e, "(rethrow)");
  } catch (InvalidCodepointException &e) {
    predstr->drop();
    if (terms != NULL) {
      terms->drop();
    }
    RETHROW(e, "(rethrow)");
  } catch (InvalidEncodingException &e) {
    predstr->drop();
    if (terms != NULL) {
      terms->drop();
    }
    RETHROW(e, "(rethrow)");
  } catch (MalformedIRIRefException &e) {
    predstr->drop();
    if (terms != NULL) {
      terms->drop();
    }
    RETHROW(e, "(rethrow)");
  } catch (TraceableException &e) {
    predstr->drop();
    if (terms != NULL) {
      terms->drop();
    }
    RETHROW(e, "(rethrow)");
  }
}

DPtr<uint8_t> *RIFTerm::toUTF8String() const throw(BadAllocException) {
  if (this->type == VARIABLE) {
    try {
      return ((RIFVar*)this->state)->toUTF8String();
    } RETHROW_BAD_ALLOC
  }
  if (this->type == CONSTANT) {
    try {
      return ((RIFConst*)this->state)->toUTF8String();
    } RETHROW_BAD_ALLOC
  }
  DPtr<uint8_t> *head = NULL;
  DPtr<RIFTerm> *terms = NULL;
  if (this->type == LIST) {
    try {
      NEW(head, MPtr<uint8_t>, 4);
    } RETHROW_BAD_ALLOC
    ascii_strcpy(head->dptr(), "List");
    terms = (DPtr<RIFTerm>*) this->state;
  } else { // FUNCTION
    func_state *f = (func_state*) this->state;
    head = f->pred.toUTF8String();
    terms = f->args;
  }
  // DO NOT drop terms.
  // head will get dropped as part of strings.
  vector<void*> strings;
  strings.push_back((void*)head);
  size_t len = head->size() + 2;
  if (terms != NULL) {
    strings.reserve(terms->size() + 1);
    if (terms->size() > 1) {
      len += terms->size() - 1;
    }
    RIFTerm *mark = terms->dptr();
    RIFTerm *end = mark + terms->size();
    for (; mark != end; ++mark) {
      DPtr<uint8_t> *str;
      try {
        str = mark->toUTF8String();
        len += str->size();
        strings.push_back((void*)str);
      } catch (BadAllocException &e) {
        vector<void*>::iterator it;
        for (it = strings.begin(); it != strings.end(); ++it) {
          ((DPtr<uint8_t>*)(*it))->drop();
        }
        RETHROW(e, "(rethrow)");
      }
    }
  }
  DPtr<uint8_t> *str;
  try {
    NEW(str, MPtr<uint8_t>, len);
  } catch (BadAllocException &e) {
    vector<void*>::iterator it;
    for (it = strings.begin(); it != strings.end(); ++it) {
      ((DPtr<uint8_t>*)(*it))->drop();
    }
    RETHROW(e, "(rethrow)");
  } catch (bad_alloc &ba) {
    vector<void*>::iterator it;
    for (it = strings.begin(); it != strings.end(); ++it) {
      ((DPtr<uint8_t>*)(*it))->drop();
    }
    THROW(BadAllocException, sizeof(MPtr<uint8_t>));
  }
  uint8_t *out = str->dptr();
  vector<void*>::iterator it = strings.begin();
  DPtr<uint8_t> *sub = (DPtr<uint8_t>*) *it;
  memcpy(out, sub->dptr(), sub->size() * sizeof(uint8_t));
  out += sub->size();
  sub->drop();
  *out = to_ascii('(');
  ++out;
  for (++it; it != strings.end(); ++it) {
    sub = (DPtr<uint8_t>*) *it;
    memcpy(out, sub->dptr(), sub->size() * sizeof(uint8_t));
    out += sub->size();
    sub->drop();
    *out = to_ascii(' ');
    ++out;
  }
  if (strings.size() > 1) {
    *(out - 1) = to_ascii(')');
  } else {
    *out = to_ascii(')');
  }
  return str;
}

RIFTerm &RIFTerm::normalize() THROWS(BadAllocException) {
  switch (this->type) {
  case VARIABLE:
    ((RIFVar*)this->state)->normalize();
    return *this;
  case CONSTANT:
    ((RIFConst*)this->state)->normalize();
    return *this;
  case LIST: {
    if (this->state == NULL) {
      return *this;
    }
    DPtr<RIFTerm> *list = (DPtr<RIFTerm>*) this->state;
    RIFTerm *begin = list->dptr();
    RIFTerm *end = begin + list->size();
    for (; begin != end; ++begin) {
      begin->normalize();
    }
    return *this;
  }
  case FUNCTION: {
    func_state *f = (func_state *) this->state;
    f->pred.normalize();
    if (f->args == NULL) {
      return *this;
    }
    RIFTerm *begin = f->args->dptr();
    RIFTerm *end = begin + f->args->size();
    for (; begin != end; ++begin) {
      begin->normalize();
    }
    return *this;
  }
  }
}
TRACE(BadAllocException, "(trace)")

bool RIFTerm::isGround() const throw() {
  switch (this->type) {
  case VARIABLE:
    return false;
  case CONSTANT:
  case LIST:
    return true;
  case FUNCTION: {
    func_state *f = (func_state *) this->state;
    if (f->args == NULL) {
      return true;
    }
    RIFTerm *begin = f->args->dptr();
    RIFTerm *end = begin + f->args->size();
    for (; begin != end; ++begin) {
      if (!begin->isGround()) {
        return false;
      }
    }
    return true;
  }
  }
}

void RIFTerm::getVars(VarSet &vars) const throw() {
  if (this->type == VARIABLE) {
    vars.insert(*((RIFVar*)this->state));
  } else if (this->type == FUNCTION) {
    func_state *f = (func_state*) this->state;
    if (f->args != NULL) {
      RIFTerm *mark = f->args->dptr();
      RIFTerm *end = mark + f->args->size();
      for (; mark != end; ++mark) {
        mark->getVars(vars);
      }
    }
  }
}

RIFVar RIFTerm::getVar() const throw(BaseException<enum RIFTermType>) {
  if (this->type != VARIABLE) {
    THROW(BaseException<enum RIFTermType>, this->type,
          "Must be type VARIABLE to call getVar.");
  }
  return *((RIFVar*)this->state);
}

RIFConst RIFTerm::getConst() const throw(BaseException<enum RIFTermType>) {
  if (this->type != CONSTANT) {
    THROW(BaseException<enum RIFTermType>, this->type,
          "Must be type CONSTANT to call getConst.");
  }
  return *((RIFConst*)this->state);
}

DPtr<RIFTerm> *RIFTerm::getItems() const throw(BaseException<enum RIFTermType>) {
  if (this->type != LIST) {
    THROW(BaseException<enum RIFTermType>, this->type,
          "Must be type LIST to call getItems.");
  }
  DPtr<RIFTerm> *p;
  if (this->state == NULL) {
    NEW(p, APtr<RIFTerm>);
    return p;
  }
  p = (DPtr<RIFTerm> *) this->state;
  p->hold();
  return p;
}

RIFConst RIFTerm::getPred() const throw(BaseException<enum RIFTermType>) {
  if (this->type != FUNCTION) {
    THROW(BaseException<enum RIFTermType>, this->type,
          "Must be type FUNCTION to call getPred.");
  }
  return ((func_state*) this->state)->pred;
}

DPtr<RIFTerm> *RIFTerm::getArgs() const
    throw(BaseException<enum RIFTermType>) {
  if (this->type != FUNCTION) {
    THROW(BaseException<enum RIFTermType>, this->type,
          "Must be type FUNCTION to call getPred.");
  }
  func_state *f = (func_state*) this->state;
  if (f->args == NULL) {
    DPtr<RIFTerm> *p;
    NEW(p, APtr<RIFTerm>);
    return p;
  }
  f->args->hold();
  return f->args;
}

RIFTerm &RIFTerm::operator=(const RIFTerm &rhs) throw(BadAllocException) {
  if (this == &rhs) {
    return *this;
  }
  if (this->type == rhs.type) {
    switch (this->type) {
    case VARIABLE:
      *((RIFVar*)this->state) = *((RIFVar*)rhs.state);
      return *this;
    case CONSTANT:
      *((RIFConst*)this->state) = *((RIFConst*)rhs.state);
      return *this;
    case FUNCTION:
      *((func_state*)this->state) = *((func_state*)rhs.state);
      return *this;
    }
  }
  void *new_state = NULL;
  switch (rhs.type) {
  case VARIABLE: {
    RIFVar *v;
    try {
      NEW(v, RIFVar, *((RIFVar*)rhs.state));
    } RETHROW_BAD_ALLOC
    new_state = v;
    break;
  }
  case CONSTANT: {
    RIFConst *c;
    try {
      NEW(c, RIFConst, *((RIFConst*)rhs.state));
    } RETHROW_BAD_ALLOC
    new_state = c;
    break;
  }
  case LIST: {
    DPtr<RIFTerm> *l = (DPtr<RIFTerm>*)rhs.state;
    if (l != NULL) {
      l->hold();
    }
    new_state = l;
    break;
  }
  case FUNCTION: {
    func_state *f;
    try {
      NEW(f, func_state, *((func_state*)rhs.state));
    } RETHROW_BAD_ALLOC
    new_state = f;
    break;
  }
  }
  switch (this->type) {
  case VARIABLE:
    DELETE((RIFVar*)this->state);
    break;
  case CONSTANT:
    DELETE((RIFConst*)this->state);
    break;
  case LIST:
    if (this->state != NULL) {
      ((DPtr<RIFTerm>*)this->state)->drop();
    }
    break;
  case FUNCTION:
    DELETE((func_state*)this->state);
    break;
  }
  this->type = rhs.type;
  this->state = new_state;
  return *this;
}

}
