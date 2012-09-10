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
#include "rif/RIFVar.h"

#include "ptr/MPtr.h"

using namespace ex;
using namespace ptr;
using namespace rif;
using namespace std;

DPtr<uint8_t> *s2p(const char *c) {
  if (c == NULL) return NULL;
  DPtr<uint8_t> *p;
  NEW(p, MPtr<uint8_t>, strlen(c));
  ascii_strcpy(p->dptr(), c);
  return p;
}

RIFVar p2v(DPtr<uint8_t> *p) {
  if (p == NULL) return RIFVar();
  RIFVar v = RIFVar::parse(p);
  p->drop();
  return v;
}

RIFVar s2v(const char *c) {
  return p2v(s2p(c));
}

bool test(RIFVar var, DPtr<uint8_t> *str, DPtr<uint8_t> *name) {
  DPtr<uint8_t> *utf8str = var.toUTF8String();
  PROG(utf8str != NULL);
  PROG(utf8str->sizeKnown());
  PROG(utf8str->size() == str->size());
  PROG(memcmp(utf8str->dptr(), str->dptr(), str->size() * sizeof(uint8_t)) == 0);
  utf8str->drop();
  str->drop();
  utf8str = var.getName();
  PROG(utf8str != NULL);
  PROG(utf8str->sizeKnown());
  PROG(utf8str->size() == name->size());
  PROG(memcmp(utf8str->dptr(), name->dptr(), name->size() * sizeof(uint8_t)) == 0);
  utf8str->drop();
  name->drop();
  var.normalize();
  PASS;
}

int main(int argc, char **argv) {
  INIT;
  TEST(test, s2v("?varname"), s2p("?varname"), s2p("varname"));
  TEST(test, s2v("?\"\\t\\r\\n\\\"\\\\ \\u0041\\u6770\\u897F\\b\\f\""),
             s2p("?\"\\t\\r\\n\\\"\\\\ A\xE6\x9D\xB0\xE8\xA5\xBF\\b\\f\""),
             s2p("\t\r\n\"\\ A\xE6\x9D\xB0\xE8\xA5\xBF\b\f"));
  FINAL;
}
