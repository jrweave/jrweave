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
#include "rdf/RDFTerm.h"

#include <cstring>
#include "ptr/MPtr.h"

using namespace iri;
using namespace lang;
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

LangTag *p2l(DPtr<uint8_t> *p) {
  LangTag *lang;
  NEW(lang, LangTag, p);
  p->drop();
  return lang;
}

LangTag *s2l(const char *cstr) {
  return p2l(s2p(cstr));
}

IRIRef *p2i(DPtr<uint8_t> *p) {
  IRIRef *iri;
  NEW(iri, IRIRef, p);
  p->drop();
  return iri;
}

IRIRef *s2i(const char *cstr) {
  return p2i(s2p(cstr));
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

bool testBnode(RDFTerm *term, DPtr<uint8_t> *label) {
  PROG(term->getType() == BNODE);
  PROG(!term->isLiteral());
  PROG(!term->isPlainLiteral());
  PROG(term->equals(*term));
  DPtr<uint8_t> *mylabel = term->getLabel();
  if (label == NULL) {
    PROG(term->isAnonymous());
    PROG(mylabel == NULL);
  } else {
    PROG(!term->isAnonymous());
    PROG(label->sizeKnown());
    PROG(mylabel->sizeKnown());
    PROG(label->size() == mylabel->size());
    PROG(memcmp(label->dptr(), mylabel->dptr(), label->size() * sizeof(uint8_t)) == 0);
    label->drop();
    mylabel->drop();
  }
  try {
    term->getIRIRef();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == BNODE);
  }
  try {
    term->getLexForm();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == BNODE);
  }
  try {
    term->getLangTag();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == BNODE);
  }
  try {
    term->getDatatype();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == BNODE);
  }
  DELETE(term);
  PASS;
}

bool testIRI(RDFTerm *term, IRIRef *iri) {
  PROG(term->getType() == IRI);
  PROG(!term->isAnonymous());
  PROG(!term->isLiteral());
  PROG(!term->isPlainLiteral());
  PROG(term->equals(*term));
  IRIRef myiri = term->getIRIRef();
  PROG(myiri.equals(*iri));
  DELETE(iri);
  try {
    term->getLabel();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == IRI);
  }
  try {
    term->getLexForm();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == IRI);
  }
  try {
    term->getLangTag();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == IRI);
  }
  try {
    term->getDatatype();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == IRI);
  }
  DELETE(term);
  PASS;
}

bool testSimpleLiteral(RDFTerm *term, DPtr<uint8_t> *lex) {
  PROG(term->getType() == SIMPLE_LITERAL);
  PROG(!term->isAnonymous());
  PROG(term->isLiteral());
  PROG(term->isPlainLiteral());
  PROG(term->equals(*term));
  DPtr<uint8_t> *mylex = term->getLexForm();
  PROG(mylex != NULL);
  PROG(mylex->sizeKnown());
  PROG(lex->size() == mylex->size());
  PROG(memcmp(lex->dptr(), mylex->dptr(), lex->size() * sizeof(uint8_t)) == 0);
  mylex->drop();
  lex->drop();
  try {
    term->getLabel();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == SIMPLE_LITERAL);
  }
  try {
    term->getIRIRef();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == SIMPLE_LITERAL);
  }
  try {
    term->getLangTag();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == SIMPLE_LITERAL);
  }
  try {
    term->getDatatype();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == SIMPLE_LITERAL);
  }
  DELETE(term);
  PASS;
}

bool testLangLiteral(RDFTerm *term, DPtr<uint8_t> *lex, LangTag *lang) {
  PROG(term->getType() == LANG_LITERAL);
  PROG(!term->isAnonymous());
  PROG(term->isLiteral());
  PROG(term->isPlainLiteral());
  PROG(term->equals(*term));
  DPtr<uint8_t> *mylex = term->getLexForm();
  PROG(mylex != NULL);
  PROG(mylex->sizeKnown());
  PROG(lex->size() == mylex->size());
  PROG(memcmp(lex->dptr(), mylex->dptr(), lex->size() * sizeof(uint8_t)) == 0);
  mylex->drop();
  lex->drop();
  LangTag mylang = term->getLangTag();
  PROG(mylang.equals(*lang));
  DELETE(lang);
  try {
    term->getLabel();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == LANG_LITERAL);
  }
  try {
    term->getIRIRef();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == LANG_LITERAL);
  }
  try {
    term->getDatatype();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == LANG_LITERAL);
  }
  DELETE(term);
  PASS;
}

bool testTypedLiteral(RDFTerm *term, DPtr<uint8_t> *lex, IRIRef *dt) {
  PROG(term->getType() == TYPED_LITERAL);
  PROG(!term->isAnonymous());
  PROG(term->isLiteral());
  PROG(!term->isPlainLiteral());
  PROG(term->equals(*term));
  DPtr<uint8_t> *mylex = term->getLexForm();
  PROG(mylex != NULL);
  PROG(mylex->sizeKnown());
  PROG(lex->size() == mylex->size());
  PROG(memcmp(lex->dptr(), mylex->dptr(), lex->size() * sizeof(uint8_t)) == 0);
  mylex->drop();
  lex->drop();
  IRIRef mydt = term->getDatatype();
  PROG(mydt.equals(*dt));
  DELETE(dt);
  try {
    term->getLabel();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == TYPED_LITERAL);
  }
  try {
    term->getIRIRef();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == TYPED_LITERAL);
  }
  try {
    term->getLangTag();
    FAIL;
  } catch (BaseException<enum RDFTermType> &e) {
    PROG(e.getCause() == TYPED_LITERAL);
  }
  DELETE(term);
  PASS;
}

bool testcmp(RDFTerm *term1, RDFTerm *term2, int cmp) {
  if (cmp < 0) {
    PROG(RDFTerm::cmp(*term1, *term2) < 0);
  } else if (cmp > 0) {
    PROG(RDFTerm::cmp(*term1, *term2) > 0);
  } else {
    PROG(RDFTerm::cmp(*term1, *term2) == 0);
  }
  DELETE(term1);
  DELETE(term2);
  PASS;
}

bool testcmpeq0(RDFTerm *term1, RDFTerm *term2, bool eq) {
  if (eq) {
    PROG(RDFTerm::cmpeq0(*term1, *term2));
  } else {
    PROG(!RDFTerm::cmpeq0(*term1, *term2));
  }
  DELETE(term1);
  DELETE(term2);
  PASS;
}

int main(int argc, char **argv) {
  INIT;

  TEST(testBnode, s2t("_:label"), s2p("label"));
  TEST(testBnode, s2t("[]"), NULL);
  TEST(testBnode, s2t("_:I haven't placed any restrictions on what can occur in a label."), s2p("I haven't placed any restrictions on what can occur in a label."));
  TEST(testIRI, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2i("http://www.cs.rpi.edu/~weavej3/foaf.rdf#me"));
  TEST(testIRI, s2t("<tag:jrweave@gmail.com,2012:contains\\>bracket>"), s2i("tag:jrweave@gmail.com,2012:contains%3Ebracket"));
  TEST(testSimpleLiteral, s2t("\"simple literal\""), s2p("simple literal"));
  TEST(testSimpleLiteral, s2t("\"\\t\\r\\n\\\"\\\\\\u0041\\u6770\\u897F\""), s2p("\t\r\n\"\\A\xE6\x9D\xB0\xE8\xA5\xBF"));
  TEST(testLangLiteral, s2t("\"simple literal\"@en-US"), s2p("simple literal"), s2l("en-US"));
  TEST(testLangLiteral, s2t("\"\\t\\r\\n\\\"\\\\\\u0041\\u6770\\u897F\"@zH-Cn"), s2p("\t\r\n\"\\A\xE6\x9D\xB0\xE8\xA5\xBF"), s2l("Zh-cN"));
  TEST(testTypedLiteral, s2t("\"1\"^^<http://www.w3.org/2001/XMLSchema#integer>"), s2p("1"), s2i("http://www.w3.org/2001/XMLSchema#integer"));
  TEST(testTypedLiteral, s2t("\"there is no validation of lexical form\"^^<http://www.w3.org/2001/XMLSchema#integer>"), s2p("there is no validation of lexical form"), s2i("http://www.w3.org/2001/XMLSchema#integer"));
  TEST(testTypedLiteral, s2t("\"\\t\\r\\n\\\"\\\\\\u0041\\u6770\\u897F\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2p("\t\r\n\"\\A\xE6\x9D\xB0\xE8\xA5\xBF"), s2i("http://www.w3.org/2001/XMLSchema#string"));

  TEST(testcmpeq0, s2t("[]"), s2t("[]"), true);
  TEST(testcmpeq0, s2t("_:label"), s2t("_:label"), true);
  TEST(testcmpeq0, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), true);
  TEST(testcmpeq0, s2t("\"simple literal\""), s2t("\"simple literal\""), true);
  TEST(testcmpeq0, s2t("\"lang literal\"@en-US"), s2t("\"lang literal\"@EN-us"), true);
  TEST(testcmpeq0, s2t("\"typed literal\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2t("\"typed literal\"^^<http://www.w3.org/2001/XMLSchema#string>"), true);

  TEST(testcmpeq0, s2t("_:label"), s2t("_:label2"), false);
  TEST(testcmpeq0, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf>"), false);
  TEST(testcmpeq0, s2t("\"simple literal\""), s2t("\"simple literal 2\""), false);
  TEST(testcmpeq0, s2t("\"lang literal\"@en-US"), s2t("\"lang literal\"@en-US-x-private"), false);
  TEST(testcmpeq0, s2t("\"typed literal\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2t("\"typed literal\"^^<http://www.w3.org/2001/XMLSchema#integer>"), false);

  TEST(testcmp, s2t("[]"), s2t("[]"), 0);
  TEST(testcmp, s2t("[]"), s2t("_:label"), -1);
  TEST(testcmp, s2t("[]"), s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), -1);
  TEST(testcmp, s2t("[]"), s2t("\"lexical form\""), -1);
  TEST(testcmp, s2t("[]"), s2t("\"lexical form\"@en-US"), -1);
  TEST(testcmp, s2t("[]"), s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), -1);

  TEST(testcmp, s2t("_:label"), s2t("[]"), 1);
  TEST(testcmp, s2t("_:label"), s2t("_:label"), 0);
  TEST(testcmp, s2t("_:label"), s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), -1);
  TEST(testcmp, s2t("_:label"), s2t("\"lexical form\""), -1);
  TEST(testcmp, s2t("_:label"), s2t("\"lexical form\"@en-US"), -1);
  TEST(testcmp, s2t("_:label"), s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), -1);

  TEST(testcmp, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2t("[]"), 1);
  TEST(testcmp, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2t("_:label"), 1);
  TEST(testcmp, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), 0);
  TEST(testcmp, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2t("\"lexical form\""), -1);
  TEST(testcmp, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2t("\"lexical form\"@en-US"), -1);
  TEST(testcmp, s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), -1);

  TEST(testcmp, s2t("\"lexical form\""), s2t("[]"), 1);
  TEST(testcmp, s2t("\"lexical form\""), s2t("_:label"), 1);
  TEST(testcmp, s2t("\"lexical form\""), s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), 1);
  TEST(testcmp, s2t("\"lexical form\""), s2t("\"lexical form\""), 0);
  TEST(testcmp, s2t("\"lexical form\""), s2t("\"lexical form\"@en-US"), -1);
  TEST(testcmp, s2t("\"lexical form\""), s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), -1);

  TEST(testcmp, s2t("\"lexical form\"@en-US"), s2t("[]"), 1);
  TEST(testcmp, s2t("\"lexical form\"@en-US"), s2t("_:label"), 1);
  TEST(testcmp, s2t("\"lexical form\"@en-US"), s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), 1);
  TEST(testcmp, s2t("\"lexical form\"@en-US"), s2t("\"lexical form\""), 1);
  TEST(testcmp, s2t("\"lexical form\"@en-US"), s2t("\"lexical form\"@en-US"), 0);
  TEST(testcmp, s2t("\"lexical form\"@en-US"), s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), -1);

  TEST(testcmp, s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2t("[]"), 1);
  TEST(testcmp, s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2t("_:label"), 1);
  TEST(testcmp, s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2t("<http://www.cs.rpi.edu/~weavej3/foaf.rdf#me>"), 1);
  TEST(testcmp, s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2t("\"lexical form\""), 1);
  TEST(testcmp, s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2t("\"lexical form\"@en-US"), 1);
  TEST(testcmp, s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), s2t("\"lexical form\"^^<http://www.w3.org/2001/XMLSchema#string>"), 0);

  FINAL;
}
