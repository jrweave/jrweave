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
#include "rif/RIFDictionary.h"

#include "rif/RIFAtomic.h"
#include "rif/RIFTerm.h"
#include "rif/RIFVar.h"

using namespace rif;

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

RIFTerm p2t(DPtr<uint8_t> *p) {
  RIFTerm t = RIFTerm::parse(p);
  p->drop();
  return t;
}

RIFTerm s2t(const char *c) {
  return p2t(s2p(c));
}

RIFVar p2v(DPtr<uint8_t> *p) {
  RIFVar v = RIFVar::parse(p);
  p->drop();
  return v;
}

RIFVar s2v(const char *c) {
  return p2v(s2p(c));
}

bool test1() {
  RIFDictionary dict;
  RIFTerm term = s2t("\"\"^^<s:>");
  uint64_t id = dict.encode(term);
  PROG(dict.getIDType(id) == TERM);
  RIFTerm found = dict.decodeTerm(id);
  PROG(term.equals(found));

  RIFTerm term2 = s2t("\"1\"^^<s:>");
  uint64_t id2 = dict.encode(term2);
  PROG(dict.getIDType(id2) == TERM);
  PROG(id + 1 == id2);
  found = dict.decodeTerm(id2);
  PROG(term2.equals(found));

  term = s2t("?var");
  id = dict.encode(term);
  PROG(dict.getIDType(id) == UNBOUND);
  try {
    found = dict.decodeTerm(id);
    FAIL;
  } catch (BaseException<uint64_t> &e) {
    cerr << endl << e.what() << endl;
    PROG(id == e.getCause());
  }

  term = s2t("\"function\"^^<s:>()");
  try {
    id = dict.encode(term);
    FAIL;
  } catch (BaseException<enum RIFTermType> &e) {
    cerr << endl << e.what() << endl;
    PROG(e.getCause() == FUNCTION);
  }

  term = s2t("List(\"\"^^<s:> \"something different\"^^<tag:jrweave@gmail.com,2012:> List(\"embedded list\"^^<s:>))");
  id = dict.encode(term);
  PROG(dict.getIDType(id) == TERM);
  found = dict.decodeTerm(id);
  PROG(term.equals(found));

  PROG(dict.getIDType(dict.encode(ATOM)) == ATOMIC_TYPE);
  PROG(dict.getIDType(dict.encode(EQUALITY)) == ATOMIC_TYPE);
  PROG(dict.getIDType(dict.encode(MEMBERSHIP)) == ATOMIC_TYPE);
  PROG(dict.getIDType(dict.encode(SUBCLASS)) == ATOMIC_TYPE);
  PROG(dict.getIDType(dict.encode(FRAME)) == ATOMIC_TYPE);
  try {
    dict.encode(EXTERNAL);
    FAIL;
  } catch (BaseException<enum RIFAtomicType> &e) {
    cerr << endl << e.what() << endl;
    PROG(e.getCause() == EXTERNAL);
  }


  PASS;
}

bool test2() {
  RIFDictionary dict;

  RIFAtomic atomic = s2a("\"\"^^<s:>( )");
  DPtr<uint64_t> *ids = dict.encode(atomic);
  PROG(ids->size() == 2);
  PROG(dict.getIDType((*ids)[0]) == ATOMIC_TYPE);
  PROG(dict.decodeType((*ids)[0]) == ATOM);
  PROG(dict.getIDType((*ids)[1]) == TERM);
  RIFAtomic found = dict.decode(ids, NULL);
  PROG(atomic.equals(found));
  ids->drop();

  atomic = s2a("\"\"^^<s:>( ?\"variable\" )");
  ids = dict.encode(atomic);
  PROG(ids->size() == 3);
  PROG(dict.getIDType((*ids)[0]) == ATOMIC_TYPE);
  PROG(dict.decodeType((*ids)[0]) == ATOM);
  PROG(dict.getIDType((*ids)[1]) == TERM);
  PROG(dict.getIDType((*ids)[2]) == UNBOUND);
  map<size_t, RIFVar> vars;
  RIFVar var = s2v("?variable");
  vars[(size_t)2] = var;
  try {
    found = dict.decode(ids, NULL);
    FAIL;
  } catch (BaseException<uint64_t> &e) {
    cerr << endl << e.what() << endl;
    PROG(dict.getIDType(e.getCause()) == UNBOUND);
  }
  found = dict.decode(ids, &vars);
  PROG(atomic.equals(found));
  ids->drop();

  atomic = s2a("External(\"\"^^<s:>( ))");
  try {
    ids = dict.encode(atomic);
    FAIL;
  } catch (BaseException<enum RIFAtomicType> &e) {
    cerr << endl << e.what() << endl;
    PROG(e.getCause() == EXTERNAL);
  }

  atomic = s2a("?var=\"\"^^<s:>");
  ids = dict.encode(atomic);
  PROG(ids->size() == 3);
  PROG(dict.getIDType((*ids)[0]) == ATOMIC_TYPE);
  PROG(dict.decodeType((*ids)[0]) == EQUALITY);
  PROG(dict.getIDType((*ids)[1]) == UNBOUND);
  PROG(dict.getIDType((*ids)[2]) == TERM);
  vars.clear();
  var = s2v("?var");
  vars[(size_t)1] = var;
  try {
    found = dict.decode(ids, NULL);
    FAIL;
  } catch (BaseException<uint64_t> &e) {
    cerr << endl << e.what() << endl;
    PROG(dict.getIDType(e.getCause()) == UNBOUND);
  }
  found = dict.decode(ids, &vars);
  PROG(atomic.equals(found));
  ids->drop();

  atomic = s2a("\"\"^^<s:>#\"\"^^<s:>");
  ids = dict.encode(atomic);
  PROG(ids->size() == 3);
  PROG(dict.getIDType((*ids)[0]) == ATOMIC_TYPE);
  PROG(dict.decodeType((*ids)[0]) == MEMBERSHIP);
  PROG(dict.getIDType((*ids)[1]) == TERM);
  PROG(dict.getIDType((*ids)[2]) == TERM);
  vars.clear();
  found = dict.decode(ids, NULL);
  PROG(atomic.equals(found));
  ids->drop();

  atomic = s2a("List(\"1\"^^<s:>)##\"\"^^<s:>");
  ids = dict.encode(atomic);
  PROG(ids->size() == 3);
  PROG(dict.getIDType((*ids)[0]) == ATOMIC_TYPE);
  PROG(dict.decodeType((*ids)[0]) == SUBCLASS);
  PROG(dict.getIDType((*ids)[1]) == TERM);
  PROG(dict.getIDType((*ids)[2]) == TERM);
  vars.clear();
  found = dict.decode(ids, NULL);
  PROG(atomic.equals(found));
  ids->drop();

  atomic = s2a("?frame[?a1->?v1 ?a2  ->  \"\"^^<s:> ?a2->?x]");
  ids = dict.encode(atomic);
  PROG(ids->size() == 3*4);
  PROG(dict.getIDType((*ids)[0]) == ATOMIC_TYPE);
  PROG(dict.decodeType((*ids)[0]) == FRAME);
  PROG(dict.getIDType((*ids)[1]) == UNBOUND);
  PROG(dict.getIDType((*ids)[2]) == UNBOUND);
  PROG(dict.getIDType((*ids)[3]) == UNBOUND);
  PROG(dict.getIDType((*ids)[4]) == ATOMIC_TYPE);
  PROG(dict.decodeType((*ids)[4]) == FRAME);
  PROG(dict.getIDType((*ids)[5]) == UNBOUND);
  PROG(dict.getIDType((*ids)[6]) == UNBOUND);
  PROG(dict.getIDType((*ids)[7]) == UNBOUND); // NOT TERM BECAUSE SLOTS SORTED
  PROG(dict.getIDType((*ids)[8]) == ATOMIC_TYPE);
  PROG(dict.decodeType((*ids)[8]) == FRAME);
  PROG(dict.getIDType((*ids)[9]) == UNBOUND);
  PROG(dict.getIDType((*ids)[10]) == UNBOUND);
  PROG(dict.getIDType((*ids)[11]) == TERM); // NOT UNBOUND BECAUSE SLOTS SORTED
  vars.clear();
  vars[(size_t)1] = s2v("?frame");
  vars[(size_t)2] = s2v("?a1");
  vars[(size_t)3] = s2v("?v1");
  vars[(size_t)5] = s2v("?frame");
  vars[(size_t)6] = s2v("?a2");
  vars[(size_t)7] = s2v("?x");
  vars[(size_t)9] = s2v("?frame");
  vars[(size_t)10] = s2v("?a2");
  try {
    found = dict.decode(ids, NULL);
    FAIL;
  } catch (BaseException<uint64_t> &e) {
    cerr << endl << e.what() << endl;
    PROG(dict.getIDType(e.getCause()) == UNBOUND);
  }
  found = dict.decode(ids, &vars);
  PROG(atomic.equals(found));
  ids->drop();


  PASS;
}

int main(int argc, char **argv) {
  INIT;
  TEST(test1);
  TEST(test2);
  FINAL;
}
