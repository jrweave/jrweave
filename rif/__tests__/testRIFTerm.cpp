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
#include "rif/RIFTerm.h"

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

RIFTerm p2t(DPtr<uint8_t> *p) {
  RIFTerm t = RIFTerm::parse(p);
  p->drop();
  return t;
}

RIFTerm s2t(const char *c) {
  return p2t(s2p(c));
}

bool testState(RIFTerm term, DPtr<uint8_t> *str, enum RIFTermType type,
    bool is_simple, bool is_ground) {
  DPtr<uint8_t> *s = term.toUTF8String();
  PROG(s->size() == str->size());
  PROG(memcmp(s->dptr(), str->dptr(), s->size()*sizeof(uint8_t)) == 0);
  s->drop();
  str->drop();
  PROG(term.getType() == type);
  PROG(term.isSimple() == is_simple);
  PROG(term.isGround() == is_ground);
  PASS;
}

bool testVar(RIFTerm term, DPtr<uint8_t> *str) {
  PROG(testState(term, str, VARIABLE, true, false));
  try {
    RIFVar v = term.getVar();
    // TODO more testing
    PROG(true);
  } catch (BaseException<enum RIFTermType> &e) {
    FAIL;
  }
  try {
    term.getConst();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == VARIABLE);
  }
  try {
    DPtr<RIFTerm> *items = term.getItems();
    items->drop();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == VARIABLE);
  }
  try {
    term.getPred();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == VARIABLE);
  }
  try {
    DPtr<RIFTerm> *args = term.getArgs();
    args->drop();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == VARIABLE);
  }
  PASS;
}

bool testConst(RIFTerm term, DPtr<uint8_t> *str) {
  PROG(testState(term, str, CONSTANT, true, true));
  try {
    RIFConst c = term.getConst();
    // TODO more testing
    PROG(true);
  } catch (BaseException<enum RIFTermType> &e) {
    FAIL;
  }
  try {
    term.getVar();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == CONSTANT);
  }
  try {
    DPtr<RIFTerm> *items = term.getItems();
    items->drop();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == CONSTANT);
  }
  try {
    term.getPred();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == CONSTANT);
  }
  try {
    DPtr<RIFTerm> *args = term.getArgs();
    args->drop();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == CONSTANT);
  }
  PASS;
}

bool testList(RIFTerm term, DPtr<uint8_t> *str, size_t n) {
  PROG(testState(term, str, LIST, false, true));
  try {
    DPtr<RIFTerm> *items = term.getItems();
    PROG(items->size() == n);
    // TODO more testing
    items->drop();
    PROG(true);
  } catch (BaseException<enum RIFTermType> &e) {
    FAIL;
  }
  try {
    term.getConst();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == LIST);
  }
  try {
    term.getVar();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == LIST);
  }
  try {
    term.getPred();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == LIST);
  }
  try {
    DPtr<RIFTerm> *args = term.getArgs();
    args->drop();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == LIST);
  }
  PASS;
}

bool testFunc(RIFTerm term, DPtr<uint8_t> *str, size_t n, bool is_ground) {
  PROG(testState(term, str, FUNCTION, false, is_ground));
  try {
    RIFConst pred = term.getPred();
    // TODO more testing
  } catch (BaseException<enum RIFTermType> &e) {
    FAIL;
  }
  try {
    DPtr<RIFTerm> *args = term.getArgs();
    PROG(args->size() == n);
    // TODO more testing
    args->drop();
  } catch (BaseException<enum RIFTermType> &e) {
    FAIL;
  }
  try {
    term.getConst();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == FUNCTION);
  }
  try {
    term.getVar();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == FUNCTION);
  }
  try {
    term.getItems();
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    PROG(e.getCause() == FUNCTION);
  }
  PASS;
}

int main(int argc, char **argv) {
  INIT;
  TEST(testVar, s2t("?var"),
                s2p("?var"));
  TEST(testVar, s2t("?\"quoted var name\""),
                s2p("?\"quoted var name\""));
  TEST(testVar, s2t("?\"variable name\""),
                s2p("?\"variable name\""));
  TEST(testConst, s2t("\"1\"^^<http://www.w3.org/2001/XMLSchema#integer>"),
                  s2p("\"1\"^^<http://www.w3.org/2001/XMLSchema#integer>"));
  TEST(testConst, s2t("\"\"^^<s:>"),
                  s2p("\"\"^^<s:>"));
  TEST(testList, s2t("List()"),
                 s2p("List()"),
                 0);
  TEST(testList, s2t("List(\t\"1\"^^<http://www.w3.org/2001/XMLSchema#integer> \"string with spaces\"^^<http://www.w3.org/2001/XMLSchema#string> )"),
                 s2p("List(\"1\"^^<http://www.w3.org/2001/XMLSchema#integer> \"string with spaces\"^^<http://www.w3.org/2001/XMLSchema#string>)"),
                 2);
  TEST(testList, s2t("List(\t\"1\"^^<http://www.w3.org/2001/XMLSchema#integer> List( \"embedded list\"^^<http://www.w3.org/2001/XMLSchema#string>\n) \"string with spaces\"^^<http://www.w3.org/2001/XMLSchema#string> )"),
                 s2p("List(\"1\"^^<http://www.w3.org/2001/XMLSchema#integer> List(\"embedded list\"^^<http://www.w3.org/2001/XMLSchema#string>) \"string with spaces\"^^<http://www.w3.org/2001/XMLSchema#string>)"),
                 3);
  TEST(testFunc, s2t("\"function\"^^<s:>()"),
                 s2p("\"function\"^^<s:>()"),
                 0, true);
  TEST(testFunc, s2t("\"http://www.w3.org/TR/xpath-functions/#func-compare\"^^<http://www.w3.org/2007/rif#iri>( \"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string> )"),
                 s2p("\"http://www.w3.org/TR/xpath-functions/#func-compare\"^^<http://www.w3.org/2007/rif#iri>(\"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string>)"),
                 2, true);
  TEST(testFunc, s2t("\"http://www.w3.org/TR/xpath-functions/#func-compare(\"^^<http://www.w3.org/2007/rif#iri>( \"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> ?var \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string> )"),
                 s2p("\"http://www.w3.org/TR/xpath-functions/#func-compare(\"^^<http://www.w3.org/2007/rif#iri>(\"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> ?var \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string>)"),
                 3, false);
  TEST(testFunc, s2t("\"http://www.w3.org/TR/xpath-functions/#func-compare\"^^<http://www.w3.org/20(07/rif#iri>( \"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string> )"),
                 s2p("\"http://www.w3.org/TR/xpath-functions/#func-compare\"^^<http://www.w3.org/20(07/rif#iri>(\"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string>)"),
                 2, true);
  TEST(testList, s2t("List(\"http://www.w3.org/TR/xpath-functions/#func-compare\"^^<http://www.w3.org/20(07/rif#iri>( \"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string> ))"),
                 s2p("List(\"http://www.w3.org/TR/xpath-functions/#func-compare\"^^<http://www.w3.org/20(07/rif#iri>(\"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string>))"),
                 1);
  TEST(testList, s2t("List(\"\"^^<scheme:> \"http://www.w3.org/TR/xpath-functions/#func-compare\"^^<http://www.w3.org/2007/rif#iri>( \"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string> ) \"1\"^^<http://www.w3.org/2001/XMLSchema#integer>)"),
                 s2p("List(\"\"^^<scheme:> \"http://www.w3.org/TR/xpath-functions/#func-compare\"^^<http://www.w3.org/2007/rif#iri>(\"first arg\"^^<http://www.w3.org/2001/XMLSchema#string> \"second arg\"^^<http://www.w3.org/2001/XMLSchema#string>) \"1\"^^<http://www.w3.org/2001/XMLSchema#integer>)"),
                 3);
  FINAL;
}
