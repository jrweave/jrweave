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
#include "rdf/RDFDictionary.h"

#include "ptr/DPtr.h"
#include "rdf/RDFTerm.h"

using namespace ptr;
using namespace rdf;
using namespace std;

DPtr<uint8_t> *s2p(const char *cstr) {
  DPtr<uint8_t> *p;
  try {
    NEW(p, MPtr<uint8_t>, strlen(cstr));
  } RETHROW_BAD_ALLOC
  ascii_strcpy(p->dptr(), cstr);
  return p;
}

RDFTerm *p2t(DPtr<uint8_t> *p) {
  RDFTerm term = RDFTerm::parse(p);
  p->drop();
  RDFTerm *termp;
  NEW(termp, RDFTerm, term);
  return termp;
}

RDFTerm *s2t(const char *cstr) {
  return p2t(s2p(cstr));
}

template<size_t N>
bool bitseq(uint64_t test, RDFID<N> id) {
  bool ret = true;
  while (test != 0) {
    bool testbit = ((test & UINT64_C(1)) != 0);
    ret &= (!testbit ^ id[0]);
    test >>= 1;
    id >>= 1;
  }
  return ret && id == RDFID<N>::zero();
}

bool testRDFID() {
  RDFID<7> id7 = RDFID<7>::zero();
  RDFID<8> id8 = RDFID<8>::zero();
  uint64_t i8 = UINT64_C(0);
  PROG(bitseq<7>(i8, id7));
  PROG(bitseq<8>(i8, id8));
  ++id7; ++id8; ++i8;
  PROG(bitseq<7>(i8, id7));
  PROG(bitseq<8>(i8, id8));
  id7 ^= ++++++++++RDFID<7>::zero();
  id8 ^= ++++++++++RDFID<8>::zero();
  i8 ^= UINT64_C(5);
  PROG(bitseq<7>(i8, id7));
  PROG(bitseq<8>(i8, id8));
  // TODO include more testing here
  PASS;
}

template<size_t N>
bool testN() {
  RDFDictionary<RDFID<N> > dict;
  RDFID<N> id;

  RDFTerm *term = s2t("<tag:jrweave@gmail.com,2012:test>");
  dict.encode(*term);
  PROG(dict.lookup(*term, id));
  RDFTerm term2;
  PROG(dict.lookup(id, term2));
  PROG(term->equals(term2));
  DELETE(term);

  RDFID<N> id2;
  term = s2t("_:blank");
  dict.encode(*term);
  PROG(dict.lookup(*term, id2));
  PROG(id != id2);
  PROG(dict.lookup(id2, term2));
  PROG(term->equals(term2));
  PROG(dict.lookup(id, term2));
  PROG(!term->equals(term2));
  DELETE(term);

  RDFID<N> id3;
  PROG((id != id3 && id2 != id3) ^ dict.lookup(id3, term2));
  
  PASS;
}

bool test8() {
  RDFDictionary<> dict;
  RDFID<8> id;
  RDFTerm *term = s2t("<tag:jrweave@gmail.com,2012:test>");
  dict.encode(*term);
  PROG(dict.lookup(*term, id));
  RDFTerm term2;
  PROG(dict.lookup(id, term2));
  PROG(term->equals(term2));
  DELETE(term);

  RDFID<8> id2;
  term = s2t("_:blank");
  dict.encode(*term);
  PROG(dict.lookup(*term, id2));
  PROG(id != id2);
  PROG(dict.lookup(id2, term2));
  PROG(term->equals(term2));
  PROG(dict.lookup(id, term2));
  PROG(!term->equals(term2));
  DELETE(term);

  RDFID<8> id3;
  PROG((id != id3 && id2 != id3) ^ dict.lookup(id3, term2));
  
  PASS;
}

int main(int argc, char **argv) {
  INIT;

  TEST(test8);
  TEST(testN<12>);
  TEST(testRDFID);

  FINAL;
}
