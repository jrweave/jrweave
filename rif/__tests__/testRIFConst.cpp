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
#include "rif/RIFConst.h"

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

IRIRef p2i(DPtr<uint8_t> *p) {
  IRIRef iri(p);
  p->drop();
  return iri;
}

IRIRef s2i(const char *c) {
  return p2i(s2p(c));
}

RIFConst p2c(DPtr<uint8_t> *p) {
  RIFConst c = RIFConst::parse(p);
  p->drop();
  return c;
}

RIFConst s2c(const char *c) {
  return p2c(s2p(c));
}

bool test(RIFConst con, DPtr<uint8_t> *str, DPtr<uint8_t> *lex, IRIRef dt) {
  DPtr<uint8_t> *utf8str = con.toUTF8String();
  PROG(utf8str != NULL);
  PROG(utf8str->sizeKnown());
  PROG(utf8str->size() == str->size());
  PROG(memcmp(utf8str->dptr(), str->dptr(), str->size() * sizeof(uint8_t)) == 0);
  utf8str->drop();
  str->drop();
  utf8str = con.getLexForm();
  PROG(utf8str != NULL);
  PROG(utf8str->sizeKnown());
  PROG(utf8str->size() == lex->size());
  PROG(memcmp(utf8str->dptr(), lex->dptr(), lex->size() * sizeof(uint8_t)) == 0);
  utf8str->drop();
  lex->drop();
  IRIRef d = con.getDatatype();
  PROG(dt.equals(d));
  con.normalize();
  PASS;
}

int main(int argc, char **argv) {
  INIT;
  TEST(test, s2c("\"\"^^<s:>"),
             s2p("\"\"^^<s:>"),
             s2p(""), s2i("s:"));
  TEST(test, s2c("\"1\"^^<http://www.w3.org/2001/XMLSchema#integer>"),
             s2p("\"1\"^^<http://www.w3.org/2001/XMLSchema#integer>"),
             s2p("1"), s2i("http://www.w3.org/2001/XMLSchema#integer"));
  TEST(test, s2c("\"\\t\\r\\n\\\"\\\\\\u0041\\u6770\\u897F\"^^<http://www.w3.org/2001/XMLSchema#string>"),
             s2p("\"\\t\\r\\n\\\"\\\\A\xE6\x9D\xB0\xE8\xA5\xBF\"^^<http://www.w3.org/2001/XMLSchema#string>"),
             s2p("\t\r\n\"\\A\xE6\x9D\xB0\xE8\xA5\xBF"),
             s2i("http://www.w3.org/2001/XMLSchema#string"));
  TEST(test, s2c("\"\\t\\r\\n\\\"\\\\\\u0041\\u6770\\u897F\\b\\f\"^^<http://www.w3.org/2001/XMLSchema#string>"),
             s2p("\"\\t\\r\\n\\\"\\\\A\xE6\x9D\xB0\xE8\xA5\xBF\\b\\f\"^^<http://www.w3.org/2001/XMLSchema#string>"),
             s2p("\t\r\n\"\\A\xE6\x9D\xB0\xE8\xA5\xBF\b\f"),
             s2i("http://www.w3.org/2001/XMLSchema#string"));
  FINAL;
}
