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

#include "test/unit.h"
#include "rif/RIFAtomic.h"

using namespace ex;
using namespace iri;
using namespace ptr;
using namespace rif;
using namespace std;

DPtr<uint8_t> *s2p(const char *c) {
  DPtr<uint8_t> *p;
  NEW(p, MPtr<uint8_t>, strlen(c));
  ascii_strcpy(p->dptr(), c);
  return p;
}

RIFAtomic p2a(DPtr<uint8_t> *p) {
  RIFAtomic a = RIFAtomic::parse(p);
  p->drop();
  return a;
}

RIFAtomic s2a(const char *c) {
  return p2a(s2p(c));
}

RIFConst p2c(DPtr<uint8_t> *p) {
  RIFConst c = RIFConst::parse(p);
  p->drop();
  return c;
}

RIFConst s2c(const char *c) {
  return p2c(s2p(c));
}

RIFTerm p2t(DPtr<uint8_t> *p) {
  RIFTerm t = RIFTerm::parse(p);
  p->drop();
  return t;
}

RIFTerm s2t(const char *c) {
  return p2t(s2p(c));
}

bool testState(RIFAtomic atomic, DPtr<uint8_t> *str, enum RIFAtomicType type,
    bool is_ground) {
  PROG(atomic.getType() == type);
  DPtr<uint8_t> *s = atomic.toUTF8String();
  PROG(s->size() == str->size());
  PROG(memcmp(s->dptr(), str->dptr(), s->size()*sizeof(uint8_t)) == 0);
  s->drop();
  str->drop();
  PROG(atomic.isGround() == is_ground);
  PASS;
}

#define INAPPLICABLE_METHOD(x, type, method, ...)   \
  try {                                             \
    x.method(__VA_ARGS__);                          \
    FAIL;                                           \
  } catch (BaseException<enum RIFAtomicType> &e) {  \
    PROG(e.getCause() == type);                     \
  }

bool testAtom(RIFAtomic atomic, DPtr<uint8_t> *str, RIFConst pred, size_t nargs, bool is_external, bool is_ground) {
  enum RIFAtomicType mytype = is_external ? EXTERNAL : ATOM;
  PROG(testState(atomic, str, mytype, is_ground));
  PROG(pred.equals(atomic.getPred()));
  DPtr<RIFTerm> *args = atomic.getArgs();
  PROG(args != NULL && args->sizeKnown() && args->size() == nargs);
  args->drop();
  TermSet terms(RIFTerm::cmplt0);
  RIFTerm term;
  INAPPLICABLE_METHOD(atomic, mytype, getEquality)
  INAPPLICABLE_METHOD(atomic, mytype, getAttrs, terms)
  INAPPLICABLE_METHOD(atomic, mytype, getValues, term, terms)
  INAPPLICABLE_METHOD(atomic, mytype, withoutAttr, term)
  INAPPLICABLE_METHOD(atomic, mytype, getObject)
  INAPPLICABLE_METHOD(atomic, mytype, getClass)
  INAPPLICABLE_METHOD(atomic, mytype, getSubclass)
  INAPPLICABLE_METHOD(atomic, mytype, getSuperclass)
  PASS;
}

bool testEquality(RIFAtomic atomic, DPtr<uint8_t> *str, RIFTerm lhs, RIFTerm rhs, bool is_ground) {
  PROG(testState(atomic, str, EQUALITY, is_ground));
  PROG(lhs.equals(atomic.getEquality().first));
  PROG(rhs.equals(atomic.getEquality().second));
  TermSet terms(RIFTerm::cmplt0);
  RIFTerm term;
  INAPPLICABLE_METHOD(atomic, EQUALITY, getPred);
  INAPPLICABLE_METHOD(atomic, EQUALITY, getArgs);
  INAPPLICABLE_METHOD(atomic, EQUALITY, getAttrs, terms)
  INAPPLICABLE_METHOD(atomic, EQUALITY, getValues, term, terms)
  INAPPLICABLE_METHOD(atomic, EQUALITY, withoutAttr, term)
  INAPPLICABLE_METHOD(atomic, EQUALITY, getObject)
  INAPPLICABLE_METHOD(atomic, EQUALITY, getClass)
  INAPPLICABLE_METHOD(atomic, EQUALITY, getSubclass)
  INAPPLICABLE_METHOD(atomic, EQUALITY, getSuperclass)
  PASS;
}

bool testMembership(RIFAtomic atomic, DPtr<uint8_t> *str, RIFTerm obj, RIFTerm cls, bool is_ground) {
  PROG(testState(atomic, str, MEMBERSHIP, is_ground));
  PROG(obj.equals(atomic.getObject()));
  PROG(cls.equals(atomic.getClass()));
  TermSet terms(RIFTerm::cmplt0);
  RIFTerm term;
  INAPPLICABLE_METHOD(atomic, MEMBERSHIP, getPred);
  INAPPLICABLE_METHOD(atomic, MEMBERSHIP, getArgs);
  INAPPLICABLE_METHOD(atomic, MEMBERSHIP, getEquality)
  INAPPLICABLE_METHOD(atomic, MEMBERSHIP, getAttrs, terms)
  INAPPLICABLE_METHOD(atomic, MEMBERSHIP, getValues, term, terms)
  INAPPLICABLE_METHOD(atomic, MEMBERSHIP, withoutAttr, term)
  INAPPLICABLE_METHOD(atomic, MEMBERSHIP, getSubclass)
  INAPPLICABLE_METHOD(atomic, MEMBERSHIP, getSuperclass)
  PASS;
}

bool testSubclass(RIFAtomic atomic, DPtr<uint8_t> *str, RIFTerm sub, RIFTerm sup, bool is_ground) {
  PROG(testState(atomic, str, SUBCLASS, is_ground));
  PROG(sub.equals(atomic.getSubclass()));
  PROG(sup.equals(atomic.getSuperclass()));
  TermSet terms(RIFTerm::cmplt0);
  RIFTerm term;
  INAPPLICABLE_METHOD(atomic, SUBCLASS, getPred);
  INAPPLICABLE_METHOD(atomic, SUBCLASS, getArgs);
  INAPPLICABLE_METHOD(atomic, SUBCLASS, getEquality)
  INAPPLICABLE_METHOD(atomic, SUBCLASS, getAttrs, terms)
  INAPPLICABLE_METHOD(atomic, SUBCLASS, getValues, term, terms)
  INAPPLICABLE_METHOD(atomic, SUBCLASS, withoutAttr, term)
  INAPPLICABLE_METHOD(atomic, SUBCLASS, getObject)
  INAPPLICABLE_METHOD(atomic, SUBCLASS, getClass)
  PASS;
}

bool testFrame(RIFAtomic atomic, DPtr<uint8_t> *str, RIFTerm obj, size_t nattrs, bool is_ground) {
  PROG(testState(atomic, str, FRAME, is_ground));
  TermSet terms(RIFTerm::cmplt0);
  RIFTerm term;
  PROG(obj.equals(atomic.getObject()));
  PROG(nattrs == atomic.getAttrs(terms));
  INAPPLICABLE_METHOD(atomic, FRAME, getPred);
  INAPPLICABLE_METHOD(atomic, FRAME, getArgs);
  INAPPLICABLE_METHOD(atomic, FRAME, getEquality)
  INAPPLICABLE_METHOD(atomic, FRAME, getClass)
  INAPPLICABLE_METHOD(atomic, FRAME, getSubclass)
  INAPPLICABLE_METHOD(atomic, FRAME, getSuperclass)
  PASS;
}


int main(int argc, char **argv) {
  INIT;
  TEST(testAtom, s2a("\"\"^^<s:>( )"), s2p("\"\"^^<s:>()"), s2c("\"\"^^<s:>"), 0, false, true);
  TEST(testAtom, s2a("\"\"^^<s:>( ?\"variable\" )"), s2p("\"\"^^<s:>(?variable)"), s2c("\"\"^^<s:>"), 1, false, false);
  TEST(testAtom, s2a("\"\"^^<s:>( ?\"variable name\" )"), s2p("\"\"^^<s:>(?\"variable name\")"), s2c("\"\"^^<s:>"), 1, false, false);
  TEST(testAtom, s2a("External(\"\"^^<s:>( ))"), s2p("External(\"\"^^<s:>())"), s2c("\"\"^^<s:>"), 0, true, true);
  TEST(testEquality, s2a("?var=\"\"^^<s:>"), s2p("?var = \"\"^^<s:>"), s2t("?var"), s2t("\"\"^^<s:>"), false);
  TEST(testEquality, s2a("?var = \"\"^^<s:>"), s2p("?var = \"\"^^<s:>"), s2t("?var"), s2t("\"\"^^<s:>"), false);
  TEST(testEquality, s2a("\"\"^^<s:?q=a>=\"\"^^<s:>"), s2p("\"\"^^<s:?q=a> = \"\"^^<s:>"), s2t("\"\"^^<s:?q=a>"), s2t("\"\"^^<s:>"), true);
  TEST(testMembership, s2a("\"\"^^<s:>#\"\"^^<s:>"), s2p("\"\"^^<s:> # \"\"^^<s:>"), s2t("\"\"^^<s:>"), s2t("\"\"^^<s:>"), true);
  TEST(testMembership, s2a("\"\"^^<s:#>#\"\"^^<s:>"), s2p("\"\"^^<s:#> # \"\"^^<s:>"), s2t("\"\"^^<s:#>"), s2t("\"\"^^<s:>"), true);
  TEST(testSubclass, s2a("List(\"1\"^^<s:>)##\"\"^^<s:>"), s2p("List(\"1\"^^<s:>) ## \"\"^^<s:>"), s2t("List(\"1\"^^<s:>)"), s2t("\"\"^^<s:>"), true);
  TEST(testFrame, s2a("?frame[?a1->?v1 ?a2  ->  \"\"^^<s:> ?a2->?x]"), s2p("?frame[?a1 -> ?v1 ?a2 -> \"\"^^<s:> ?a2 -> ?x]"), s2t("?frame"), 2, false);
  FINAL; 
}
