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
#include "lang/LangRange.h"

#include <algorithm>
#include "lang/LangTag.h"
#include "ptr/DPtr.h"
#include "ptr/MPtr.h"

using namespace lang;
using namespace ptr;
using namespace std;

void print(DPtr<uint8_t> *ascii) {
  size_t i;
  cerr << "[PRINT] ";
  for (i = 0; i < ascii->size(); i++) {
    cerr << (char) (*ascii)[i];
  }
  cerr << endl << dec;
}

DPtr<uint8_t> *str2ptr(const char *str) {
  DPtr<uint8_t> *p;
  NEW(p, MPtr<uint8_t>, strlen(str));
  copy(str, str + strlen(str), p->dptr());
  return p;
}

LangTag *ptr2tag(DPtr<uint8_t> *ptr) THROWS(MalformedLangTagException) {
  LangTag *tag;
  NEW(tag, LangTag, ptr);
  ptr->drop();
  return tag;
}
TRACE(MalformedLangTagException, "(trace)")

LangTag *str2tag(const char *str) THROWS(MalformedLangTagException) {
  return ptr2tag(str2ptr(str));
}
TRACE(MalformedLangTagException, "(trace)")

LangRange *ptr2rng(DPtr<uint8_t> *ptr) THROWS(MalformedLangRangeException) {
  LangRange *rng;
  NEW(rng, LangRange, ptr);
  ptr->drop();
  return rng;
}
TRACE(MalformedLangRangeException, "(trace)")

LangRange *str2rng(const char *str) THROWS(MalformedLangRangeException) {
  return ptr2rng(str2ptr(str));
}
TRACE(MalformedLangRangeException, "(trace)")

bool equiv(LangRange *rng, DPtr<uint8_t> *str, bool basic) {
  DPtr<uint8_t> *rngstr = rng->getASCIIString();
  PROG(rngstr != NULL);
  PROG(str != NULL);
  PROG(rngstr->size() == str->size());
  PROG(equal(str->dptr(), str->dptr() + str->size(), rngstr->dptr()));
  PROG((basic && rng->isBasic()) || (!basic && !rng->isBasic()));
  DELETE(rng);
  str->drop();
  rngstr->drop();
  PASS;
}

bool wellformed(DPtr<uint8_t> *well) {
  try {
    LangRange *rng = ptr2rng(well);
    DELETE(rng);
    PASS;
  } catch (MalformedLangRangeException &e) {
    cerr << e.what() << endl;
    well->drop();
    FAIL;
  }
}

bool malformed(DPtr<uint8_t> *mal) {
  try {
    LangRange *rng = ptr2rng(mal);
    DELETE(rng);
    FAIL;
  } catch (MalformedLangRangeException &e) {
    cerr << e.what() << endl;
    mal->drop();
    PASS;
  }
}

bool equal(LangRange *l, LangRange *r) {
  PROG(l->equals(*r));
  DELETE(l);
  DELETE(r);
  PASS;
}

bool unequal(LangRange *l, LangRange *r) {
  PROG(!l->equals(*r));
  DELETE(l);
  DELETE(r);
  PASS;
}

bool match(LangRange *rng, LangTag *tag) {
  PROG(rng->matches(tag));
  DELETE(rng);
  DELETE(tag);
  PASS;
}

bool nomatch(LangRange *rng, LangTag *tag) {
  PROG(!rng->matches(tag));
  DELETE(rng);
  DELETE(tag);
  PASS;
}

int main (int argc, char **argv) {
  INIT;

  LangRange *rng;

  // default construction
  NEW(rng, LangRange);
  TEST(equiv, rng, str2ptr("*"), true);

  TEST(wellformed, str2ptr("de-DE"));
  TEST(malformed, str2ptr("malformed"));
  TEST(malformed, str2ptr("mal_formed"));
  TEST(wellformed, str2ptr("well-formed"));
  TEST(malformed, str2ptr("1-2"));
  TEST(wellformed, str2ptr("a-2"));

  TEST(equiv, str2rng("de-DE"), str2ptr("de-DE"), true);
  TEST(equiv, str2rng("de-*-DE"), str2ptr("de-*-DE"), false);

  TEST(equal, str2rng("de-DE"), str2rng("DE-de"));
  TEST(unequal, str2rng("de-DE"), str2rng("de-*-DE"));

  TEST(match, str2rng("de-de"), str2tag("de-DE-1996"));
  TEST(nomatch, str2rng("de-de"), str2tag("de-Deva"));
  TEST(nomatch, str2rng("de-de"), str2tag("de-Latn-DE"));

  TEST(match, str2rng("de-*-DE"), str2tag("de-DE"));
  TEST(match, str2rng("de-*-DE"), str2tag("de-de"));
  TEST(match, str2rng("de-*-DE"), str2tag("de-Latn-DE"));
  TEST(match, str2rng("de-*-DE"), str2tag("de-Latf-DE"));
  TEST(match, str2rng("de-*-DE"), str2tag("de-DE-x-goethe"));
  TEST(match, str2rng("de-*-DE"), str2tag("de-Latn-DE-1996"));
  TEST(match, str2rng("de-*-1996"), str2tag("de-Latn-DE-1996"));
  TEST(match, str2rng("de-*-DE"), str2tag("de-Deva-DE"));
  TEST(nomatch, str2rng("de-*-DE"), str2tag("de"));
  TEST(nomatch, str2rng("de-*-DE"), str2tag("de-x-DE"));
  TEST(nomatch, str2rng("de-*-DE"), str2tag("de-Deva"));

  TEST(match, str2rng("*"), str2tag("i-default"));
  TEST(match, str2rng("i"), str2tag("i-default"));
  TEST(match, str2rng("i-*"), str2tag("i-default"));
  TEST(match, str2rng("i-default"), str2tag("i-default"));
  TEST(match, str2rng("i-*-default"), str2tag("i-default"));
  TEST(match, str2rng("i-*-*-*-default-*-*-*-*"), str2tag("i-default"));
  TEST(nomatch, str2rng("*-i-*-*-*-default-*-*-*-*"), str2tag("i-default"));
  TEST(match, str2rng("*-default"), str2tag("i-default"));
  TEST(match, str2rng("*-*-*-default"), str2tag("i-default"));
  TEST(match, str2rng("*-x-private"), str2tag("en-x-private")); 
  TEST(match, str2rng("*-x-*"), str2tag("en-x-private")); 
  TEST(match, str2rng("*-x"), str2tag("en-x-private")); 
  TEST(match, str2rng("*-private"), str2tag("x-private")); 
  TEST(nomatch, str2rng("en-US-x-private"), str2tag("en-US-u-extend")); 
  TEST(nomatch, str2rng("en-US-u-extend-x-private"), str2tag("en-US-u-extend")); 
 
  FINAL;
}
