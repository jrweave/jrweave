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

LangTag *ptr2tag(DPtr<uint8_t> *ptr) throw(MalformedLangTagException) {
  LangTag *tag;
  NEW(tag, LangTag, ptr);
  ptr->drop();
  return tag;
}

LangTag *str2tag(const char *str) throw(MalformedLangTagException) {
  return ptr2tag(str2ptr(str));
}

bool equiv(LangTag *tag, DPtr<uint8_t> *full, DPtr<uint8_t> *lang,
    DPtr<uint8_t> *scr, DPtr<uint8_t> *reg, DPtr<uint8_t> *vars,
    DPtr<uint8_t> *exts, DPtr<uint8_t> *priv) {
  DPtr<uint8_t> *part = tag->getASCIIString();
  PROG(full != NULL);
  PROG(part->size() == full->size());
  PROG(memcmp(part->dptr(), full->dptr(), part->size() * sizeof(uint8_t)) == 0);
  full->drop();
  part->drop();
  part = tag->getPart(LANGUAGE);
  if (part == NULL) {
    PROG(lang == NULL);
  } else {
    PROG(lang != NULL);
    PROG(part->size() == lang->size());
    PROG(memcmp(part->dptr(), lang->dptr(), part->size() * sizeof(uint8_t)) == 0);
    lang->drop();
    part->drop();
  }
  part = tag->getPart(SCRIPT);
  if (part == NULL) {
    PROG(scr == NULL);
  } else {
    PROG(scr != NULL);
    PROG(part->size() == scr->size());
    PROG(memcmp(part->dptr(), scr->dptr(), part->size() * sizeof(uint8_t)) == 0);
    scr->drop();
    part->drop();
  }
  part = tag->getPart(REGION);
  if (part == NULL) {
    PROG(reg == NULL);
  } else {
    PROG(reg != NULL);
    PROG(part->size() == reg->size());
    PROG(memcmp(part->dptr(), reg->dptr(), part->size() * sizeof(uint8_t)) == 0);
    reg->drop();
    part->drop();
  }
  part = tag->getPart(VARIANTS);
  if (part == NULL) {
    PROG(vars == NULL);
  } else {
    PROG(vars != NULL);
    PROG(part->size() == vars->size());
    PROG(memcmp(part->dptr(), vars->dptr(), part->size() * sizeof(uint8_t)) == 0);
    vars->drop();
    part->drop();
  }
  part = tag->getPart(EXTENSIONS);
  if (part == NULL) {
    PROG(exts == NULL);
  } else {
    PROG(exts != NULL);
    PROG(part->size() == exts->size());
    PROG(memcmp(part->dptr(), exts->dptr(), part->size() * sizeof(uint8_t)) == 0);
    exts->drop();
    part->drop();
  }
  part = tag->getPart(PRIVATE_USE);
  if (part == NULL) {
    PROG(priv == NULL);
  } else {
    PROG(priv != NULL);
    PROG(part->size() == priv->size());
    PROG(memcmp(part->dptr(), priv->dptr(), part->size() * sizeof(uint8_t)) == 0);
    priv->drop();
    part->drop();
  }
  DELETE(tag);
  PASS;
}

bool atypical(LangTag *tag, bool privateuse, bool grandfathered,
    bool regular, bool irregular) {
  PROG(!privateuse || !grandfathered);
  PROG(!grandfathered || regular || irregular);
  PROG((!regular && !irregular) || grandfathered);
  PROG(!regular || !irregular);
  PROG(tag->isPrivateUse() == privateuse);
  PROG(tag->isGrandfathered() == grandfathered);
  PROG(tag->isRegularGrandfathered() == regular);
  PROG(tag->isIrregularGrandfathered() == irregular);
  DELETE(tag);
  PASS;
}

bool malformed(DPtr<uint8_t> *mal) {
  try {
    LangTag *tag = ptr2tag(mal);
    DELETE(tag);
    FAIL;
  } catch (MalformedLangTagException &e) {
    cerr << e.what() << endl;
    mal->drop();
    PASS;
  }
}

bool equal(LangTag *l, LangTag *r) {
  PROG(l->equals(*r));
  DELETE(l);
  DELETE(r);
  PASS;
}

bool unequal(LangTag *l, LangTag *r) {
  PROG(!l->equals(*r));
  DELETE(l);
  DELETE(r);
  PASS;
}

int main (int argc, char **argv) {
  INIT;

  LangTag *tag;

  // default construction
  NEW(tag, LangTag);
  TEST(equiv, tag, str2ptr("i-default"), NULL, NULL, NULL, NULL, NULL, NULL);
  NEW(tag, LangTag);
  TEST(atypical, tag, false, true, false, true);

  // test cases from appendix a
  TEST(equiv, str2tag("de"), str2ptr("de"), str2ptr("de"), NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("fr"), str2ptr("fr"), str2ptr("fr"), NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("ja"), str2ptr("ja"), str2ptr("ja"), NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("i-enochian"), str2ptr("i-enochian"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-enochian"), false, true, false, true);
  TEST(equiv, str2tag("zh-Hant"), str2ptr("zh-Hant"), str2ptr("zh"), str2ptr("Hant"), NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("zh-Hans"), str2ptr("zh-Hans"), str2ptr("zh"), str2ptr("Hans"), NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("sr-Cyrl"), str2ptr("sr-Cyrl"), str2ptr("sr"), str2ptr("Cyrl"), NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("sr-Latn"), str2ptr("sr-Latn"), str2ptr("sr"), str2ptr("Latn"), NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("zh-cmn-Hans-CN"), str2ptr("zh-cmn-Hans-CN"), str2ptr("zh-cmn"), str2ptr("Hans"), str2ptr("CN"), NULL, NULL, NULL);
  TEST(equiv, str2tag("cmn-Hans-CN"), str2ptr("cmn-Hans-CN"), str2ptr("cmn"), str2ptr("Hans"), str2ptr("CN"), NULL, NULL, NULL);
  TEST(equiv, str2tag("zh-yue-HK"), str2ptr("zh-yue-HK"), str2ptr("zh-yue"), NULL, str2ptr("HK"), NULL, NULL, NULL);
  TEST(equiv, str2tag("yue-HK"), str2ptr("yue-HK"), str2ptr("yue"), NULL, str2ptr("HK"), NULL, NULL, NULL);
  TEST(equiv, str2tag("zh-Hans-CN"), str2ptr("zh-Hans-CN"), str2ptr("zh"), str2ptr("Hans"), str2ptr("CN"), NULL, NULL, NULL);
  TEST(equiv, str2tag("sr-Latn-RS"), str2ptr("sr-Latn-RS"), str2ptr("sr"), str2ptr("Latn"), str2ptr("RS"), NULL, NULL, NULL);
  TEST(equiv, str2tag("sl-rozaj"), str2ptr("sl-rozaj"), str2ptr("sl"), NULL, NULL, str2ptr("rozaj"), NULL, NULL);
  TEST(equiv, str2tag("sl-rozaj-biske"), str2ptr("sl-rozaj-biske"), str2ptr("sl"), NULL, NULL, str2ptr("rozaj-biske"), NULL, NULL);
  TEST(equiv, str2tag("sl-nedis"), str2ptr("sl-nedis"), str2ptr("sl"), NULL, NULL, str2ptr("nedis"), NULL, NULL);
  TEST(equiv, str2tag("de-CH-1901"), str2ptr("de-CH-1901"), str2ptr("de"), NULL, str2ptr("CH"), str2ptr("1901"), NULL, NULL);
  TEST(equiv, str2tag("sl-IT-nedis"), str2ptr("sl-IT-nedis"), str2ptr("sl"), NULL, str2ptr("IT"), str2ptr("nedis"), NULL, NULL);
  TEST(equiv, str2tag("hy-Latn-IT-arevela"), str2ptr("hy-Latn-IT-arevela"), str2ptr("hy"), str2ptr("Latn"), str2ptr("IT"), str2ptr("arevela"), NULL, NULL);
  TEST(equiv, str2tag("de-DE"), str2ptr("de-DE"), str2ptr("de"), NULL, str2ptr("DE"), NULL, NULL, NULL);
  TEST(equiv, str2tag("en-US"), str2ptr("en-US"), str2ptr("en"), NULL, str2ptr("US"), NULL, NULL, NULL);
  TEST(equiv, str2tag("es-419"), str2ptr("es-419"), str2ptr("es"), NULL, str2ptr("419"), NULL, NULL, NULL);
  TEST(equiv, str2tag("de-CH-x-phonebk"), str2ptr("de-CH-x-phonebk"), str2ptr("de"), NULL, str2ptr("CH"), NULL, NULL, str2ptr("x-phonebk"));
  TEST(equiv, str2tag("az-Arab-x-AZE-derbend"), str2ptr("az-Arab-x-AZE-derbend"), str2ptr("az"), str2ptr("Arab"), NULL, NULL, NULL, str2ptr("x-AZE-derbend"));
  TEST(atypical, str2tag("x-whatever"), true, false, false, false);
  TEST(equiv, str2tag("x-whatever"), str2ptr("x-whatever"), NULL, NULL, NULL, NULL, NULL, str2ptr("x-whatever"));
  TEST(equiv, str2tag("qaa-Qaaa-QM-x-southern"), str2ptr("qaa-Qaaa-QM-x-southern"), str2ptr("qaa"), str2ptr("Qaaa"), str2ptr("QM"), NULL, NULL, str2ptr("x-southern"));
  TEST(equiv, str2tag("de-Qaaa"), str2ptr("de-Qaaa"), str2ptr("de"), str2ptr("Qaaa"), NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("sr-Latn-QM"), str2ptr("sr-Latn-QM"), str2ptr("sr"), str2ptr("Latn"), str2ptr("QM"), NULL, NULL, NULL);
  TEST(equiv, str2tag("sr-Qaaa-RS"), str2ptr("sr-Qaaa-RS"), str2ptr("sr"), str2ptr("Qaaa"), str2ptr("RS"), NULL, NULL, NULL);
  TEST(equiv, str2tag("en-US-u-islamcal"), str2ptr("en-US-u-islamcal"), str2ptr("en"), NULL, str2ptr("US"), NULL, str2ptr("u-islamcal"), NULL);
  TEST(equiv, str2tag("zh-CN-a-myext-x-private"), str2ptr("zh-CN-a-myext-x-private"), str2ptr("zh"), NULL, str2ptr("CN"), NULL, str2ptr("a-myext"), str2ptr("x-private"));
  TEST(equiv, str2tag("en-a-myext-b-another"), str2ptr("en-a-myext-b-another"), str2ptr("en"), NULL, NULL, NULL, str2ptr("a-myext-b-another"), NULL);

  // grandfathered
  TEST(atypical, str2tag("en-GB-oed"), false, true, false, true);
  TEST(equiv, str2tag("en-GB-oed"), str2ptr("en-GB-oed"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("i-ami"), str2ptr("i-ami"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-ami"), false, true, false, true);
  TEST(equiv, str2tag("i-bnn"), str2ptr("i-bnn"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-bnn"), false, true, false, true);
  TEST(equiv, str2tag("i-default"), str2ptr("i-default"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-default"), false, true, false, true);
  TEST(equiv, str2tag("i-enochian"), str2ptr("i-enochian"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-enochian"), false, true, false, true);
  TEST(equiv, str2tag("i-hak"), str2ptr("i-hak"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-hak"), false, true, false, true);
  TEST(equiv, str2tag("i-klingon"), str2ptr("i-klingon"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-klingon"), false, true, false, true);
  TEST(equiv, str2tag("i-lux"), str2ptr("i-lux"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-lux"), false, true, false, true);
  TEST(equiv, str2tag("i-mingo"), str2ptr("i-mingo"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-mingo"), false, true, false, true);
  TEST(equiv, str2tag("i-navajo"), str2ptr("i-navajo"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-navajo"), false, true, false, true);
  TEST(equiv, str2tag("i-pwn"), str2ptr("i-pwn"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-pwn"), false, true, false, true);
  TEST(equiv, str2tag("i-tao"), str2ptr("i-tao"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-tao"), false, true, false, true);
  TEST(equiv, str2tag("i-tay"), str2ptr("i-tay"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-tay"), false, true, false, true);
  TEST(equiv, str2tag("i-tsu"), str2ptr("i-tsu"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("i-tsu"), false, true, false, true);
  TEST(atypical, str2tag("sgn-BE-FR"), false, true, false, true);
  TEST(equiv, str2tag("sgn-BE-FR"), str2ptr("sgn-BE-FR"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("sgn-BE-NL"), str2ptr("sgn-BE-NL"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("sgn-BE-NL"), false, true, false, true);
  TEST(equiv, str2tag("sgn-CH-DE"), str2ptr("sgn-CH-DE"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("sgn-CH-DE"), false, true, false, true);
  TEST(equiv, str2tag("art-lojban"), str2ptr("art-lojban"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("art-lojban"), false, true, true, false);
  TEST(equiv, str2tag("cel-gaulish"), str2ptr("cel-gaulish"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("cel-gaulish"), false, true, true, false);
  TEST(equiv, str2tag("no-bok"), str2ptr("no-bok"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("no-bok"), false, true, true, false);
  TEST(equiv, str2tag("no-nyn"), str2ptr("no-nyn"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("no-nyn"), false, true, true, false);
  TEST(equiv, str2tag("zh-guoyu"), str2ptr("zh-guoyu"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("zh-guoyu"), false, true, true, false);
  TEST(equiv, str2tag("zh-hakka"), str2ptr("zh-hakka"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("zh-hakka"), false, true, true, false);
  TEST(equiv, str2tag("zh-min"), str2ptr("zh-min"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("zh-min"), false, true, true, false);
  TEST(equiv, str2tag("zh-min-nan"), str2ptr("zh-min-nan"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("zh-min-nan"), false, true, true, false);
  TEST(equiv, str2tag("zh-xiang"), str2ptr("zh-xiang"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(atypical, str2tag("zh-xiang"), false, true, true, false);

  // malformed
  TEST(malformed, str2ptr(""));
  TEST(malformed, str2ptr("en-"));
  TEST(malformed, str2ptr("type-anything-you-want"));
  TEST(malformed, str2ptr("using_invalid_characters"));

  // normalization
  TEST(equiv, str2tag("EN-US-B-WARBLE-A-BABBLE"), str2ptr("EN-US-B-WARBLE-A-BABBLE"), str2ptr("EN"), NULL, str2ptr("US"), NULL, str2ptr("B-WARBLE-A-BABBLE"), NULL);
  TEST(equiv, str2tag("EN-US-B-WARBLE-A-BABBLE")->normalize(), str2ptr("en-US-a-babble-b-warble"), str2ptr("en"), NULL, str2ptr("US"), NULL, str2ptr("a-babble-b-warble"), NULL);
  TEST(equiv, str2tag("NO-NYN"), str2ptr("NO-NYN"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("NO-NYN")->normalize(), str2ptr("nn"), str2ptr("nn"), NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("I-KLINGON"), str2ptr("I-KLINGON"), NULL, NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("I-KLINGON")->normalize(), str2ptr("tlh"), str2ptr("tlh"), NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("sgn-es"), str2ptr("sgn-es"), str2ptr("sgn"), NULL, str2ptr("es"), NULL, NULL, NULL);
  TEST(equiv, str2tag("sgn-es")->normalize(), str2ptr("ssp"), str2ptr("ssp"), NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("zh-hak"), str2ptr("zh-hak"), str2ptr("zh-hak"), NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("zh-hak")->normalize(), str2ptr("hak"), str2ptr("hak"), NULL, NULL, NULL, NULL, NULL);
  TEST(equiv, str2tag("zh-hak-X-wHo-CaReS"), str2ptr("zh-hak-X-wHo-CaReS"), str2ptr("zh-hak"), NULL, NULL, NULL, NULL, str2ptr("X-wHo-CaReS"));
  TEST(equiv, str2tag("zh-hak-X-wHo-CaReS")->normalize(), str2ptr("hak-x-who-cares"), str2ptr("hak"), NULL, NULL, NULL, NULL, str2ptr("x-who-cares"));
  TEST(equiv, str2tag("en-b-ccc-bbb-a-aaa-X-xyz"), str2ptr("en-b-ccc-bbb-a-aaa-X-xyz"), str2ptr("en"), NULL, NULL, NULL, str2ptr("b-ccc-bbb-a-aaa"), str2ptr("X-xyz"));
  TEST(equiv, str2tag("en-b-ccc-bbb-a-aaa-X-xyz")->normalize(), str2ptr("en-a-aaa-b-ccc-bbb-x-xyz"), str2ptr("en"), NULL, NULL, NULL, str2ptr("a-aaa-b-ccc-bbb"), str2ptr("x-xyz"));
  TEST(equiv, str2tag("en-BU"), str2ptr("en-BU"), str2ptr("en"), NULL, str2ptr("BU"), NULL, NULL, NULL);
  TEST(equiv, str2tag("en-BU")->normalize(), str2ptr("en-MM"), str2ptr("en"), NULL, str2ptr("MM"), NULL, NULL, NULL);

  // extlangify
  TEST(equiv, str2tag("hak-CN"), str2ptr("hak-CN"), str2ptr("hak"), NULL, str2ptr("CN"), NULL, NULL, NULL);
  TEST(equiv, str2tag("hak-CN")->normalize(), str2ptr("hak-CN"), str2ptr("hak"), NULL, str2ptr("CN"), NULL, NULL, NULL);
  TEST(equiv, str2tag("hak-CN")->extlangify(), str2ptr("zh-hak-CN"), str2ptr("zh-hak"), NULL, str2ptr("CN"), NULL, NULL, NULL);
  TEST(equiv, str2tag("zh-hak-hans-cn-pinyin-v-and-hi-u-hello-world-x-x"), str2ptr("zh-hak-hans-cn-pinyin-v-and-hi-u-hello-world-x-x"), str2ptr("zh-hak"), str2ptr("hans"), str2ptr("cn"), str2ptr("pinyin"), str2ptr("v-and-hi-u-hello-world"), str2ptr("x-x"));
  TEST(equiv, str2tag("zh-hak-hans-cn-pinyin-v-and-hi-u-hello-world-x-x")->normalize(), str2ptr("hak-Hans-CN-pinyin-u-hello-world-v-and-hi-x-x"), str2ptr("hak"), str2ptr("Hans"), str2ptr("CN"), str2ptr("pinyin"), str2ptr("u-hello-world-v-and-hi"), str2ptr("x-x"));
  TEST(equiv, str2tag("zh-hak-hans-cn-pinyin-v-and-hi-u-hello-world-x-x")->extlangify(), str2ptr("zh-hak-Hans-CN-pinyin-u-hello-world-v-and-hi-x-x"), str2ptr("zh-hak"), str2ptr("Hans"), str2ptr("CN"), str2ptr("pinyin"), str2ptr("u-hello-world-v-and-hi"), str2ptr("x-x"));
  TEST(equiv, str2tag("x-notation")->extlangify(), str2ptr("x-notation"), NULL, NULL, NULL, NULL, NULL, str2ptr("x-notation"));

  // equality; expect case-INsensitive, lexical equality
  TEST(equal, str2tag("hak-CN"), str2tag("HAK-cn"));
  TEST(unequal, str2tag("hak-CN"), str2tag("zh-hak-CN"));
  TEST(equal, str2tag("ZH-HAK-cn")->normalize(), str2tag("hak-CN"));
  TEST(unequal, str2tag("zh-hak-CN")->normalize(), str2tag("zh-hak-CN")->extlangify());
  TEST(unequal, str2tag("Hak-cn"), str2tag("zh-hak-CN")->extlangify());

  FINAL;
}
