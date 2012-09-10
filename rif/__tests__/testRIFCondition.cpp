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
#include "rif/RIFCondition.h"

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

RIFCondition p2f(DPtr<uint8_t> *p) {
  RIFCondition f = RIFCondition::parse(p);
  p->drop();
  return f;
}

RIFCondition s2f(const char *c) {
  return p2f(s2p(c));
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

bool testState(RIFCondition cond, DPtr<uint8_t> *str, enum RIFCondType type,
    bool is_ground) {
  PROG(cond.getType() == type);
  DPtr<uint8_t> *s = cond.toUTF8String();
  PROG(s->size() == str->size());
  PROG(memcmp(s->dptr(), str->dptr(), s->size()*sizeof(uint8_t)) == 0);
  s->drop();
  str->drop();
  PROG(cond.isGround() == is_ground);
  PASS;
}

#define INAPPLICABLE_METHOD(x, type, method, ...)   \
  try {                                             \
    x.method(__VA_ARGS__);                          \
    FAIL;                                           \
  } catch (BaseException<enum RIFAtomicType> &e) {  \
    PROG(e.getCause() == type);                     \
  }

int main(int argc, char **argv) {
  INIT;
  TEST(testState, s2f("\"\"^^<s:>( ?\"variable name\" )"), s2p("\"\"^^<s:>(?\"variable name\")"), ATOMIC, false);
  TEST(testState, s2f("And( )"), s2p("And()"), CONJUNCTION, true);
  TEST(testState, s2f("And(\"filled\"^^<s:>(?\"variable\" ) )"), s2p("And(\"filled\"^^<s:>(?variable))"), CONJUNCTION, false);
  TEST(testState, s2f("And( \" filled with spaces \"^^<s:>( ?\"variable name\" ) )"), s2p("And(\" filled with spaces \"^^<s:>(?\"variable name\"))"), CONJUNCTION, false);
  TEST(testState, s2f("And( \" filled with spaces \"^^<s:>( ?\"variable name\" ) ?a=?b ?m#?c ?c##?s)"), s2p("And(\" filled with spaces \"^^<s:>(?\"variable name\") ?a = ?b ?m # ?c ?c ## ?s)"), CONJUNCTION, false);
  TEST(testState, s2f("And( \" filled with spaces \"^^<s:>( ?\"variable name\" ) ?a = ?b ?m # ?c ?c ## ?s)"), s2p("And(\" filled with spaces \"^^<s:>(?\"variable name\") ?a = ?b ?m # ?c ?c ## ?s)"), CONJUNCTION, false);
  TEST(testState, s2f("Or( )"), s2p("Or()"), DISJUNCTION, true);
  TEST(testState, s2f("Or( \" filled with spaces \"^^<s:>( ?\"variable name\" ) ?a = ?b ?m # ?c ?c ## ?s)"), s2p("Or(\" filled with spaces \"^^<s:>(?\"variable name\") ?a = ?b ?m # ?c ?c ## ?s)"), DISJUNCTION, false);
  TEST(testState, s2f("Not( \" filled with spaces \"^^<s:>( ?\"variable name\" ) )"), s2p("Not(\" filled with spaces \"^^<s:>(?\"variable name\"))"), NEGATION, false);
  TEST(testState, s2f("Exists ?a ( \" filled with spaces \"^^<s:>( ?\"variable name\" ) )"), s2p("Exists ?a (\" filled with spaces \"^^<s:>(?\"variable name\"))"), EXISTENTIAL, false);
  TEST(testState, s2f("Exists ?\"variable name\" ( \" filled with spaces \"^^<s:>( ?\"variable name\" ) )"), s2p("Exists ?\"variable name\" (\" filled with spaces \"^^<s:>(?\"variable name\"))"), EXISTENTIAL, true);
  TEST(testState, s2f("Exists ?\"variable name\" ( Or(INeg(\" filled with spaces \"^^<s:>( ?\"variable name\" )) ?a=?b) )"), s2p("Exists ?\"variable name\" (Or(Not(\" filled with spaces \"^^<s:>(?\"variable name\")) ?a = ?b))"), EXISTENTIAL, false);
  FINAL;
}
