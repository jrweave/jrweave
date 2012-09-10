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

#include "rif/RIFAtomic.h"

#include "ptr/APtr.h"
#include "ptr/MPtr.h"

namespace rif {

using namespace ptr;
using namespace std;

RIFAtomic::RIFAtomic(const RIFConst &pred, DPtr<RIFTerm> *args, bool external)
    throw(SizeUnknownException, BadAllocException)
    : type(external ? EXTERNAL : ATOM) {
  atom_state *a;
  try {
    NEW(a, atom_state, pred, args);
  } JUST_RETHROW(SizeUnknownException, "(rethrow)")
    RETHROW_BAD_ALLOC
  this->state = a;
}

RIFAtomic::RIFAtomic(const enum RIFAtomicType type, const RIFTerm &lhs,
                     const RIFTerm &rhs)
    throw(BadAllocException, BaseException<enum RIFAtomicType>)
    : type(type) {
  if (type == ATOM || type == FRAME || type == EXTERNAL) {
    THROW(BaseException<enum RIFAtomicType>, type);
  }
  pair<RIFTerm, RIFTerm> *p;
  try {
    NEW(p, WHOLE(pair<RIFTerm, RIFTerm>), lhs, rhs);
  } RETHROW_BAD_ALLOC
  this->state = p;
}

RIFAtomic::RIFAtomic(const RIFTerm &obj, const SlotMap &slots)
    throw(BadAllocException)
    : type(FRAME) {
  frame_state *f;
  try {
    NEW(f, frame_state, obj, slots);
  } RETHROW_BAD_ALLOC
  this->state = f;
}

RIFAtomic::RIFAtomic(const RIFAtomic &copy) throw(BadAllocException)
    : type(copy.type) {
  switch (this->type) {
    case ATOM:
    case EXTERNAL: {
      atom_state *a;
      atom_state *copya = (atom_state*)copy.state;
      try {
        NEW(a, atom_state, *copya);
      } RETHROW_BAD_ALLOC
      this->state = a;
      break;
    }
    case EQUALITY:
    case MEMBERSHIP:
    case SUBCLASS: {
      pair<RIFTerm, RIFTerm> *p;
      pair<RIFTerm, RIFTerm> *copyp = (pair<RIFTerm, RIFTerm>*) copy.state;
      try {
        NEW(p, WHOLE(pair<RIFTerm, RIFTerm>), *copyp);
      } RETHROW_BAD_ALLOC
      this->state = p;
      break;
    }
    case FRAME: {
      frame_state *f;
      frame_state *copyf = (frame_state*) copy.state;
      try {
        NEW(f, frame_state, *copyf);
      } RETHROW_BAD_ALLOC
      this->state = f;
      break;
    }
  }
}

RIFAtomic::~RIFAtomic() throw() {
  switch (this->type) {
    case ATOM:
    case EXTERNAL:
      DELETE((atom_state*)this->state);
      break;
    case EQUALITY:
    case MEMBERSHIP:
    case SUBCLASS:
      DELETE((pair<RIFTerm, RIFTerm>*)this->state);
      break;
    case FRAME:
      DELETE((frame_state*)this->state);
      break;
  }
}

int RIFAtomic::cmp(const RIFAtomic &a1, const RIFAtomic &a2) throw() {
  if (a1.type != a2.type) {
    return ((int)a1.type) - ((int)a2.type);
  }
  switch (a1.type) {
  case ATOM:
  case EXTERNAL: {
    RIFConst pred1 = a1.getPred();
    RIFConst pred2 = a2.getPred();
    int c = RIFConst::cmp(pred1, pred2);
    if (c != 0) {
      return c;
    }
    DPtr<RIFTerm> *args1 = a1.getArgs();
    DPtr<RIFTerm> *args2 = a2.getArgs();
    if (args1->size() != args2->size()) {
      c = args1->size() < args2->size() ? -1 : 1;
      args1->drop();
      args2->drop();
      return c;
    }
    RIFTerm *begin = args1->dptr();
    RIFTerm *end = begin + args1->size();
    RIFTerm *mark = args2->dptr();
    for (; begin != end; ++begin) {
      c = RIFTerm::cmp(*begin, *mark);
      if (c != 0) {
        args1->drop();
        args2->drop();
        return c;
      }
      ++mark;
    }
    args1->drop();
    args2->drop();
    return 0;
  }
  case EQUALITY:
  case MEMBERSHIP:
  case SUBCLASS: {
    pair<RIFTerm, RIFTerm> *p1 = (pair<RIFTerm, RIFTerm>*)a1.state;
    pair<RIFTerm, RIFTerm> *p2 = (pair<RIFTerm, RIFTerm>*)a2.state;
    if (a1.type == EQUALITY) {
      if (RIFTerm::cmplt0(p1->second, p1->first)) {
        swap(p1->second, p1->first);
      }
      if (RIFTerm::cmplt0(p2->second, p2->first)) {
        swap(p2->second, p2->first);
      }
    }
    int c = RIFTerm::cmp(p1->first, p2->first);
    if (c != 0) {
      return c;
    }
    return RIFTerm::cmp(p1->second, p2->second);
  }
  case FRAME: {
    RIFTerm obj1 = a1.getObject();
    RIFTerm obj2 = a2.getObject();
    int c = RIFTerm::cmp(obj1, obj2);
    if (c != 0) {
      return c;
    }
    TermSet attr1(RIFTerm::cmplt0);
    TermSet attr2(RIFTerm::cmplt0);
    a1.getAttrs(attr1);
    a2.getAttrs(attr2);
    if (attr1.size() != attr2.size()) {
      return attr1.size() < attr2.size() ? -1 : 1;
    }
    TermSet::const_iterator it1 = attr1.begin();
    TermSet::const_iterator it2 = attr2.begin();
    for (; it1 != attr1.end(); ++it1) {
      c = RIFTerm::cmp(*it1, *it2);
      if (c != 0) {
        return c;
      }
      TermSet val1(RIFTerm::cmplt0);
      TermSet val2(RIFTerm::cmplt0);
      a1.getValues(*it1, val1);
      a2.getValues(*it2, val2);
      if (val1.size() != val2.size()) {
        return val1.size() < val2.size() ? -1 : 1;
      }
      TermSet::const_iterator itt1 = val1.begin();
      TermSet::const_iterator itt2 = val2.begin();
      for (; itt1 != val1.end(); ++itt1) {
        c = RIFTerm::cmp(*itt1, *itt2);
        if (c != 0) {
          return c;
        }
        ++itt2;
      }
      ++it2;
    }
    return 0;
  }
  }
}

RIFAtomic RIFAtomic::parse(DPtr<uint8_t> *utf8str)
    throw(BadAllocException, SizeUnknownException,
          InvalidCodepointException, InvalidEncodingException,
          MalformedIRIRefException, TraceableException) {
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  const uint8_t *mark = begin;
  DPtr<uint8_t> *sub = NULL;
  bool external = false;
  do {
    for (; mark != end && *mark != to_ascii('(') &&
           *mark != to_ascii('=') && *mark != to_ascii('#') &&
           *mark != to_ascii('['); ++mark) {
      // find possible delimiter
    }
    if (mark == end) {
      THROW(TraceableException, "Invalid RIFAtomic string.");
    }
    const uint8_t *backup = mark - 1;
    for (; backup != begin && is_space(*backup); --backup) {
      // find end of first part
    }
    ++backup;
    sub = utf8str->sub(begin - utf8str->dptr(), backup - begin);
    if (*mark == to_ascii('(') && !external) {
      external = (ascii_strncmp(begin, "External", 8) == 0);
      if (external) {
        sub->drop();
        sub = NULL;
        for (begin = ++mark; begin != end && is_space(*begin); ++begin) {
          // find first non-space character in External(...)
        }
        for (--end; end != begin && is_space(*end); --end) {
          // find last non-space character
        }
        if (*end != to_ascii(')')) {
          THROW(TraceableException, "No closing ')' on EXTERNAL.");
        }
        if (end != begin) {
          for (--end; end != begin && is_space(*end); --end) {
            // find last non-space character in External(...)
          }
          ++end;
        }
        mark = begin;
        continue;
      }
    }
    RIFTerm term;
    try {
      term = RIFTerm::parse(sub);
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
      sub = NULL;
      ++mark;
      continue; // ignore and try again
    } catch (TraceableException &e) {
      sub->drop();
      sub = NULL;
      ++mark;
      continue; // ignore and try again
    }
    sub->drop();
    sub = NULL;
    if (*mark == to_ascii('(')) {
      if (term.getType() != CONSTANT) {
        THROW(TraceableException, "ATOM/EXTERNAL must have CONSTANT predicate");
      }
      for (--end; end != begin && is_space(*end); --end) {
        // find last non-space character
      }
      if (end == begin || *end != to_ascii(')')) {
        THROW(TraceableException, "No closing ')' on ATOM.");
      }
      vector<RIFTerm> args;
      for (begin = mark + 1; begin != end && is_space(*begin); ++begin) {
        // find beginning of arguments
      }
      if (begin != end) {
        for (--end; end != begin && is_space(*end); --end) {
          // find last non-space char in argument list
        }
        ++end;
      }
      mark = begin;
      while (mark != end) {
        for (; mark != end && !is_space(*mark); ++mark) {
          // find potential end of argument
        }
        sub = utf8str->sub(begin - utf8str->dptr(), mark - begin);
        try {
          RIFTerm arg = RIFTerm::parse(sub);
          sub->drop();
          sub = NULL;
          args.push_back(arg);
          for (; mark != end && is_space(*mark); ++mark) {
            // find next argument, or end
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
          sub = NULL;
          for (; mark != end && is_space(*mark); ++mark) {
            // find next argument, or end
          }
          continue; // ignore and try again
        } catch (TraceableException &e) {
          sub->drop();
          sub = NULL;
          for (; mark != end && is_space(*mark); ++mark) {
            // find next argument, or end
          }
          continue; // ignore and try again
        }
      }
      if (begin != end) {
        THROW(TraceableException, "Unrecognized arguments of ATOM/EXTERNAL.");
      }
      RIFConst pred = term.getConst();
      DPtr<RIFTerm> *pargs = NULL;
      try {
        NEW(pargs, APtr<RIFTerm>, args.size());
      } RETHROW_BAD_ALLOC
      vector<RIFTerm>::const_iterator it;
      RIFTerm *parg = pargs->dptr();
      for (it = args.begin(); it != args.end(); ++it) {
        *parg = *it;
        ++parg;
      }
      try {
        RIFAtomic rifatomic(pred, pargs, external);
        pargs->drop();
        return rifatomic;
      } catch (BadAllocException &e) {
        pargs->drop();
        RETHROW(e, "(rethrow)");
      }
    } else if (external) {
      THROW(TraceableException, "External(...) must contain ATOM.");
    } else if (*mark == to_ascii('=') || *mark == to_ascii('#')) {
      enum RIFAtomicType type;
      if (*mark == to_ascii('=')) {
        type = EQUALITY;
      } else if (mark + 1 == end || mark[1] != to_ascii('#')) {
        type = MEMBERSHIP;
      } else {
        type = SUBCLASS;
        ++mark;
      }
      for (++mark; mark != end && is_space(*mark); ++mark) {
        // find beginning of rhs
      }
      if (mark == end) {
        THROW(TraceableException,
              "Missing right-hand side of EQUALITY, MEMBERSHIP, or SUBCLASS.");
      }
      RIFTerm term2;
      sub = utf8str->sub(mark - utf8str->dptr(), end - mark);
      try {
        term2 = RIFTerm::parse(sub);
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
        RETHROW(e, "(rethrow)");
      } catch (TraceableException &e) {
        sub->drop();
        RETHROW(e, "(rethrow)");
      }
      sub->drop();
      return RIFAtomic(type, term, term2);
    } else if (*mark == to_ascii('[')) {
      SlotMap slots(RIFTerm::cmplt0);
      for (begin = mark + 1; begin != end && is_space(*begin); ++begin) {
        // find first slot
      }
      if (begin == end) {
        THROW(TraceableException, "No closing ']' on FRAME.");
      }
      for (--end; end != begin && is_space(*end); --end) {
        // find end of frame
      }
      if (*end != to_ascii(']')) {
        THROW(TraceableException, "No closing ']' on FRAME.");
      }
      if (end != begin) {
        for (--end; end != begin && is_space(*end); --end) {
          // find end of last slot
        }
        ++end;
      }
      mark = begin;
      while (mark != end) {
        for (; mark + 1 != end &&
               (mark[0] != to_ascii('-') || mark[1] != to_ascii('>'));
             ++mark) {
          // find "->"
        }
        if (mark + 1 == end) {
          THROW(TraceableException, "Incomplete slot in FRAME.");
        }
        const uint8_t *backup = mark - 1;
        for (; backup != begin && is_space(*backup); --backup) {
          // find end of attribute
        }
        ++backup;
        sub = utf8str->sub(begin - utf8str->dptr(), backup - begin);
        RIFTerm attr;
        try {
          attr = RIFTerm::parse(sub);
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
          sub = NULL;
          mark += 2;
          continue; // ignore and try again
        } catch (TraceableException &e) {
          sub->drop();
          sub = NULL;
          mark += 2;
          continue; // ignore and try again
        }
        sub->drop();
        sub = NULL;
        begin = mark + 2;
        for (; begin != end && is_space(*begin); ++begin) {
          // find beginning of value
        }
        mark = begin;
        while (mark != end) {
          for (; mark != end && !is_space(*mark); ++mark) {
            // find potential end of value
          }
          sub = utf8str->sub(begin - utf8str->dptr(), mark - begin);
          RIFTerm val;
          try {
            val = RIFTerm::parse(sub);
            sub->drop();
            sub = NULL;
            slots.insert(pair<RIFTerm, RIFTerm>(attr, val));
            break;
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
            sub = NULL;
            // ignore and try again
          } catch (TraceableException &e) {
            sub->drop();
            sub = NULL;
            // ignore and try again
          }
          for  (; mark != end && is_space(*mark); ++mark) {
            // go to next non-space char
          }
        }
        begin = mark;
        if (begin != end) {
          for (++begin; begin != end && is_space(*begin); ++begin) {
            // find next slot
          }
          mark = begin;
        }
      }
      try {
        return RIFAtomic(term, slots);
      } JUST_RETHROW(BadAllocException, "(rethrow)")
    }
  } while (sub == NULL);
}

DPtr<uint8_t> *RIFAtomic::toUTF8String() const throw(BadAllocException) {
  vector<void*> subs;
  DPtr<uint8_t> *str = NULL;
  size_t len = 0;
  switch (this->type) {
    case EXTERNAL:
      len += 10;
      // fall-through
    case ATOM: {
      len += 2;
      atom_state *a = (atom_state*) this->state;
      try {
        str = a->pred.toUTF8String();
      } JUST_RETHROW(BadAllocException, "(rethrow)")
      subs.push_back((void*)str);
      len += str->size();
      str = NULL;
      if (a->args != NULL && a->args->size() > 0) {
        len += a->args->size() - 1;
        RIFTerm *arg = a->args->dptr();
        RIFTerm *end = arg + a->args->size();
        for (; arg != end; ++arg) {
          try {
            str = arg->toUTF8String();
          } catch (BadAllocException &e) {
            vector<void*>::iterator it = subs.begin();
            for (; it != subs.end(); ++it) {
              ((DPtr<uint8_t>*)(*it))->drop();
            }
            RETHROW(e, "(rethrow)");
          }
          subs.push_back((void*)str);
          len += str->size();
          str = NULL;
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
      } catch (bad_alloc &e) {
        vector<void*>::iterator it = subs.begin();
        for (; it != subs.end(); ++it) {
          ((DPtr<uint8_t>*)(*it))->drop();
        }
        THROW(BadAllocException, sizeof(MPtr<uint8_t>));
      }
      uint8_t *bytes = str->dptr();
      if (this->type == EXTERNAL) {
        ascii_strcpy(bytes, "External(");
        bytes += 9;
      }
      DPtr<uint8_t> *sub = (DPtr<uint8_t>*)subs[0];
      subs[0] = NULL;
      memcpy(bytes, sub->dptr(), sub->size() * sizeof(uint8_t));
      bytes += sub->size();
      sub->drop();
      *bytes = to_ascii('(');
      ++bytes;
      size_t i;
      for (i = 1; i < subs.size(); ++i) {
        sub = (DPtr<uint8_t>*)subs[i];
        subs[i] = NULL;
        memcpy(bytes, sub->dptr(), sub->size() * sizeof(uint8_t));
        bytes += sub->size();
        sub->drop();
        if (i < subs.size() - 1) {
          *bytes = to_ascii(' ');
          ++bytes;
        }
      }
      *bytes = to_ascii(')');
      if (this->type == EXTERNAL) {
        bytes[1] = to_ascii(')');
      }
      return str;
    }
    case EQUALITY:
    case MEMBERSHIP:
    case SUBCLASS: {
      len += this->type == SUBCLASS ? 4 : 3;
      pair<RIFTerm, RIFTerm> *p = (pair<RIFTerm, RIFTerm>*) this->state;
      DPtr<uint8_t> *lhs;
      try {
        lhs = p->first.toUTF8String();
      } JUST_RETHROW(BadAllocException, "(rethrow)")
      DPtr<uint8_t> *rhs;
      try {
        rhs = p->second.toUTF8String();
      } catch (BadAllocException &e) {
        lhs->drop();
        RETHROW(e, "(rethrow)");
      }
      len += lhs->size() + rhs->size();
      try {
        NEW(str, MPtr<uint8_t>, len);
      } catch (BadAllocException &e) {
        lhs->drop();
        rhs->drop();
        RETHROW(e, "(rethrow)");
      } catch (bad_alloc &e) {
        lhs->drop();
        rhs->drop();
        THROW(BadAllocException, sizeof(MPtr<uint8_t>));
      }
      uint8_t *bytes = str->dptr();
      memcpy(bytes, lhs->dptr(), lhs->size() * sizeof(uint8_t));
      bytes += lhs->size();
      lhs->drop();
      if (this->type == EQUALITY) {
        ascii_strcpy(bytes, " = ");
        bytes += 3;
      } else if (this->type == MEMBERSHIP) {
        ascii_strcpy(bytes, " # ");
        bytes += 3;
      } else if (this->type == SUBCLASS) {
        ascii_strcpy(bytes, " ## ");
        bytes += 4;
      }
      memcpy(bytes, rhs->dptr(), rhs->size() * sizeof(uint8_t));
      rhs->drop();
      return str;
    }
    case FRAME: {
      len += 2;
      frame_state *f = (frame_state*)this->state;
      try {
        str = f->object.toUTF8String();
      } JUST_RETHROW(BadAllocException, "(rethrow)")
      len += str->size();
      subs.push_back((void*)str);
      len += f->slots.size() - 1;
      SlotMap::const_iterator slot = f->slots.begin();
      for (; slot != f->slots.end(); ++slot) {
        len += 4;
        try {
          str = slot->first.toUTF8String();
          len += str->size();
          subs.push_back((void*)str);
          str = slot->second.toUTF8String();
          len += str->size();
          subs.push_back((void*)str);
        } catch (BadAllocException &e) {
          vector<void*>::iterator it = subs.begin();
          for (; it != subs.end(); ++it) {
            ((DPtr<uint8_t>*)(*it))->drop();
          }
          RETHROW(e, "(rethrow)");
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
      } catch (bad_alloc &e) {
        vector<void*>::iterator it = subs.begin();
        for (; it != subs.end(); ++it) {
          ((DPtr<uint8_t>*)(*it))->drop();
        }
        THROW(BadAllocException, sizeof(MPtr<uint8_t>));
      }
      uint8_t *bytes = str->dptr();
      DPtr<uint8_t> *sub = (DPtr<uint8_t>*) subs[0];
      memcpy(bytes, sub->dptr(), sub->size() * sizeof(uint8_t));
      bytes += sub->size();
      sub->drop();
      *bytes = to_ascii('[');
      ++bytes;
      size_t i;
      for (i = 1; i < subs.size(); i += 2) {
        sub = (DPtr<uint8_t>*) subs[i];
        memcpy(bytes, sub->dptr(), sub->size() * sizeof(uint8_t));
        bytes += sub->size();
        sub->drop();
        ascii_strcpy(bytes, " -> ");
        bytes += 4;
        sub = (DPtr<uint8_t>*) subs[i+1];
        memcpy(bytes, sub->dptr(), sub->size() * sizeof(uint8_t));
        bytes += sub->size();
        sub->drop();
        if (i < subs.size() - 2) {
          *bytes = to_ascii(' ');
          ++bytes;
        }
      }
      *bytes = to_ascii(']');
      return str;
    }
  }
  THROW(TraceableException, "Unhandled case; this should never happen.");
}

RIFAtomic &RIFAtomic::normalize() THROWS(BadAllocException) {
  switch (this->type) {
  case ATOM:
  case EXTERNAL: {
    atom_state *a = (atom_state*)this->state;
    a->pred.normalize();
    if (a->args != NULL) {
      RIFTerm *mark = a->args->dptr();
      RIFTerm *end = mark + a->args->size();
      for (; mark != end; ++mark) {
        mark->normalize();
      }
    }
    return *this;
  }
  case EQUALITY: {
    pair<RIFTerm, RIFTerm> *p = (pair<RIFTerm, RIFTerm>*)this->state;
    p->first.normalize();
    p->second.normalize();
    if (RIFTerm::cmplt0(p->second, p->first)) {
      swap(p->second, p->first);
    }
    return *this;
  }
  case MEMBERSHIP:
  case SUBCLASS: {
    pair<RIFTerm, RIFTerm> *p = (pair<RIFTerm, RIFTerm>*)this->state;
    p->first.normalize();
    p->second.normalize();
    return *this;
  }
  case FRAME: {
    frame_state *f = (frame_state*) this->state;
    f->object.normalize();
    SlotMap normslots(RIFTerm::cmplt0);
    SlotMap::const_iterator it = f->slots.begin();
    for (; it != f->slots.end(); ++it) {
      RIFTerm attr = it->first;
      RIFTerm value = it->second;
      attr.normalize();
      value.normalize();
      normslots.insert(pair<RIFTerm, RIFTerm>(attr, value));
    }
    f->slots.swap(normslots);
    return *this;
  }
  }
}
TRACE(BadAllocException, "(trace)")

bool RIFAtomic::isGround() const throw() {
  switch (this->type) {
  case ATOM:
  case EXTERNAL: {
    atom_state *a = (atom_state*) this->state;
    if (a->args != NULL) {
      RIFTerm *mark = a->args->dptr();
      RIFTerm *end = mark + a->args->size();
      for (; mark != end; ++mark) {
        if (!mark->isGround()) {
          return false;
        }
      }
    }
    return true;
  }
  case EQUALITY:
  case MEMBERSHIP:
  case SUBCLASS: {
    pair<RIFTerm, RIFTerm> *p = (pair<RIFTerm, RIFTerm>*) this->state;
    return p->first.isGround() && p->second.isGround();
  }
  case FRAME: {
    frame_state *f = (frame_state*) this->state;
    if (!f->object.isGround()) {
      return false;
    }
    SlotMap::const_iterator it = f->slots.begin();
    for (; it != f->slots.end(); ++it) {
      if (!it->first.isGround() || !it->second.isGround()) {
        return false;
      }
    }
    return true;
  }
  }
}

void RIFAtomic::getVars(VarSet &vars) const throw() {
  switch (this->type) {
  case ATOM:
  case EXTERNAL: {
    atom_state *a = (atom_state*) this->state;
    if (a->args != NULL) {
      RIFTerm *mark = a->args->dptr();
      RIFTerm *end = mark + a->args->size();
      for (; mark != end; ++mark) {
        mark->getVars(vars);
      }
    }
    return;
  }
  case EQUALITY:
  case MEMBERSHIP:
  case SUBCLASS: {
    pair<RIFTerm, RIFTerm> *p = (pair<RIFTerm, RIFTerm>*) this->state;
    p->first.getVars(vars);
    p->second.getVars(vars);
    return;
  }
  case FRAME: {
    frame_state *f = (frame_state*) this->state;
    f->object.getVars(vars);
    SlotMap::const_iterator it = f->slots.begin();
    for (; it != f->slots.end(); ++it) {
      it->first.getVars(vars);
      it->second.getVars(vars);
    }
    return;
  }
  }
}

RIFConst RIFAtomic::getPred() const throw(BaseException<enum RIFAtomicType>) {
  if (this->type != ATOM && this->type != EXTERNAL) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be ATOM or EXTERNAL to call getPred().");
  }
  return ((atom_state*)this->state)->pred;
}

DPtr<RIFTerm> *RIFAtomic::getArgs() const
    throw(BaseException<enum RIFAtomicType>) {
  if (this->type != ATOM && this->type != EXTERNAL) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be ATOM or EXTERNAL to call getArgs().");
  }
  atom_state *a = (atom_state*) this->state;
  if (a->args == NULL) {
    DPtr<RIFTerm> *p;
    NEW(p, MPtr<RIFTerm>);
    return p;
  }
  a->args->hold();
  return a->args;
}

pair<RIFTerm, RIFTerm> RIFAtomic::getEquality() const
    throw(BaseException<enum RIFAtomicType>) {
  if (this->type != EQUALITY) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be EQUALITY to call getEquality().");
  }
  return *((pair<RIFTerm, RIFTerm>*)this->state);
}

size_t RIFAtomic::getAttrs(TermSet &attrs) const
    throw(BaseException<enum RIFAtomicType>) {
  if (this->type != FRAME) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be FRAME to call getAttrs(...).");
  }
  frame_state *f = (frame_state*) this->state;
  SlotMap::const_iterator begin = f->slots.begin();
  SlotMap::const_iterator end = f->slots.end();
  for (; begin != end; ++begin) {
    attrs.insert(begin->first);
  }
  return attrs.size();
}

size_t RIFAtomic::getValues(const RIFTerm &attr, TermSet &vals) const
    throw(BaseException<enum RIFAtomicType>) {
  if (this->type != FRAME) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be FRAME to call getValues(...).");
  }
  frame_state *f = (frame_state*) this->state;
  pair<SlotMap::const_iterator, SlotMap::const_iterator> range
      = f->slots.equal_range(attr);
  for (; range.first != range.second; ++(range.first)) {
    vals.insert(range.first->second);
  }
  return vals.size();
}

RIFAtomic RIFAtomic::withoutAttr(const RIFTerm &attr) const
    throw(BaseException<enum RIFAtomicType>) {
  if (this->type != FRAME) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be FRAME to call withoutAttr(...).");
  }
  frame_state *f = (frame_state*) this->state;
  SlotMap new_slots(RIFTerm::cmplt0);
  pair<SlotMap::const_iterator, SlotMap::const_iterator> range
      = f->slots.equal_range(attr);
  SlotMap::const_iterator begit = f->slots.begin();
  SlotMap::const_iterator endit = f->slots.end();
  new_slots.insert(begit, range.first);
  new_slots.insert(range.second, endit);
  return RIFAtomic(f->object, new_slots);
}

RIFTerm RIFAtomic::getObject() const throw(BaseException<enum RIFAtomicType>) {
  if (this->type == FRAME) {
    return ((frame_state*)this->state)->object;
  }
  if (this->type == MEMBERSHIP) {
    return ((pair<RIFTerm, RIFTerm>*)this->state)->first;
  }
  THROW(BaseException<enum RIFAtomicType>, this->type,
        "Type must be MEMBERSHIP or FRAME to call getObject().");
}

RIFTerm RIFAtomic::getClass() const throw(BaseException<enum RIFAtomicType>) {
  if (this->type != MEMBERSHIP) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be MEMBERSHIP to call getClass().");
  }
  return ((pair<RIFTerm, RIFTerm>*)this->state)->second;
}

RIFTerm RIFAtomic::getSubclass() const
    throw(BaseException<enum RIFAtomicType>) {
  if (this->type != SUBCLASS) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be SUBCLASS to call getSubclass().");
  }
  return ((pair<RIFTerm, RIFTerm>*)this->state)->first;
}

RIFTerm RIFAtomic::getSuperclass() const
    throw(BaseException<enum RIFAtomicType>) {
  if (this->type != SUBCLASS) {
    THROW(BaseException<enum RIFAtomicType>, this->type,
          "Type must be SUBCLASS to call getSuperclass().");
  }
  return ((pair<RIFTerm, RIFTerm>*)this->state)->second;
}

RIFAtomic &RIFAtomic::operator=(const RIFAtomic &rhs)
    throw(BadAllocException) {
  if (this == &rhs) {
    return *this;
  }
  if ((this->type == ATOM || this->type == EXTERNAL)
      && (rhs.type == ATOM || rhs.type == EXTERNAL)) {
    *((atom_state*)this->state) = *((atom_state*)rhs.state);
    this->type = rhs.type;
    return *this;
  }
  if (this->type == FRAME && rhs.type == FRAME) {
    *((frame_state*)this->state) = *((frame_state*)rhs.state);
    return *this;
  }
  if (this->type != ATOM && this->type != EXTERNAL && this->type != FRAME
      && rhs.type != ATOM && rhs.type != EXTERNAL && rhs.type != FRAME) {
    *((pair<RIFTerm, RIFTerm>*)this->state) =
        *((pair<RIFTerm, RIFTerm>*)rhs.state);
    this->type = rhs.type;
    return *this;
  }
  void *new_state = NULL;
  switch (rhs.type) {
    case ATOM:
    case EXTERNAL: {
      atom_state *a = NULL;
      try {
        NEW(a, atom_state, *((atom_state*)rhs.state));
      } RETHROW_BAD_ALLOC
      new_state = a;
      break;
    }
    case EQUALITY:
    case MEMBERSHIP:
    case SUBCLASS: {
      pair<RIFTerm, RIFTerm> *p = NULL;
      try {
        NEW(p, WHOLE(pair<RIFTerm, RIFTerm>), *((pair<RIFTerm, RIFTerm>*)rhs.state));
      } RETHROW_BAD_ALLOC
      new_state = p;
      break;
    }
    case FRAME: {
      frame_state *f = NULL;
      try {
        NEW(f, frame_state, *((frame_state*)rhs.state));
      } RETHROW_BAD_ALLOC
      new_state = f;
      break;
    }
  }
  switch (this->type) {
    case ATOM:
    case EXTERNAL:
      DELETE((atom_state*)this->state);
      break;
    case EQUALITY:
    case MEMBERSHIP:
    case SUBCLASS:
      DELETE((pair<RIFTerm, RIFTerm> *)this->state);
      break;
    case FRAME:
      DELETE((frame_state*)this->state);
      break;
  }
  this->state = new_state;
  this->type = rhs.type;
  return *this;
}

}
