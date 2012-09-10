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

#include "rdf/RDFTerm.h"

#include <vector>
#include "sys/char.h"
#include "ucs/nf.h"
#include "ucs/utf.h"

namespace rdf {

using namespace ex;
using namespace iri;
using namespace lang;
using namespace ptr;
using namespace std;
using namespace ucs;

RDFTerm::RDFTerm(DPtr<uint8_t> *label) throw(SizeUnknownException)
    : type(BNODE), iri(NULL), lang(NULL), normalized(false) {
  if (label != NULL && !label->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  this->bytes = label;
  if (this->bytes != NULL) {
    this->bytes->hold();
  }
}

RDFTerm::RDFTerm(const IRIRef &iriref)
    throw(BaseException<IRIRef>, BadAllocException)
    : type(IRI), lang(NULL), bytes(NULL), normalized(false) {
  if (iriref.isRelativeRef()) {
    THROW(BaseException<IRIRef>, iriref, "IRIRef must not be relative.");
  }
  try {
    NEW(this->iri, IRIRef, iriref);
  } RETHROW_BAD_ALLOC
}

RDFTerm::RDFTerm(DPtr<uint8_t> *utf8lex, const LangTag *lang)
    throw(SizeUnknownException, BaseException<void*>, BadAllocException,
          InvalidEncodingException, InvalidCodepointException)
    : type(lang == NULL ? SIMPLE_LITERAL : LANG_LITERAL), iri(NULL),
      lang(NULL), normalized(false) {
  if (utf8lex == NULL) {
    THROW(BaseException<void*>, (void*) NULL, "utf8lex must not be NULL.");
  }
  if (!utf8lex->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  try {
    utf8validate(utf8lex);
  } JUST_RETHROW(InvalidEncodingException, "(rethrow)")
    JUST_RETHROW(InvalidCodepointException, "(rethrow)")
  if (lang != NULL) {
    try {
      NEW(this->lang, LangTag, *lang);
    } RETHROW_BAD_ALLOC
  }
  this->bytes = utf8lex;
  this->bytes->hold();
}

RDFTerm::RDFTerm(DPtr<uint8_t> *utf8lex, const IRIRef &datatype)
    throw(SizeUnknownException, BaseException<void*>, BadAllocException,
          InvalidEncodingException, InvalidCodepointException)
    : type(TYPED_LITERAL), lang(NULL), normalized(false) {
  if (utf8lex == NULL) {
    THROW(BaseException<void*>, (void*) NULL, "utf8lex must not be NULL.");
  }
  if (!utf8lex->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  try {
    utf8validate(utf8lex);
  } JUST_RETHROW(InvalidEncodingException, "(rethrow)")
    JUST_RETHROW(InvalidCodepointException, "(rethrow)")
  try {
    NEW(this->iri, IRIRef, datatype);
  } RETHROW_BAD_ALLOC
  this->bytes = utf8lex;
  this->bytes->hold();
}

RDFTerm::RDFTerm(const RDFTerm &copy) throw(BadAllocException)
    : type(copy.type), normalized(copy.normalized) {
  if (copy.iri == NULL) {
    this->iri = NULL;
  } else {
    try {
      NEW(this->iri, IRIRef, *(copy.iri));
    } RETHROW_BAD_ALLOC
  }
  if (copy.lang == NULL) {
    this->lang = NULL;
  } else {
    try {
      NEW(this->lang, LangTag, *(copy.lang));
    } catch (BadAllocException &ba) {
      if (this->iri != NULL) {
        DELETE(this->iri);
      }
      RETHROW(ba, "(rethrow)");
    } catch (bad_alloc &ba) {
      if (this->iri != NULL) {
        DELETE(this->iri);
      }
      THROW(BadAllocException, sizeof(LangTag));
    }
  }
  this->bytes = copy.bytes;
  if (this->bytes != NULL) {
    this->bytes->hold();
  }
}

RDFTerm::~RDFTerm() throw() {
  if (this->iri != NULL) {
    DELETE(this->iri);
  }
  if (this->lang != NULL) {
    DELETE(this->lang);
  }
  if (this->bytes != NULL) {
    this->bytes->drop();
  }
}

// Order
// 1. Anonymous BNODEs
// 2. Labelled BNODEs ordered on bytes (memcmp)
// 3. IRIs based on IRIRef::cmp
// 4. SIMPLE_LITERALS ordered on bytes (memcmp)
// 5. LANG_LITERALS ordered first on lang, then on lex
// 6. TYPED_LITERALS ordered first on datatype, then on lex
int RDFTerm::cmp(const RDFTerm &term1, const RDFTerm &term2) throw() {
  if (&term1 == &term2) {
    return 0;
  }
  enum RDFTermType type = term1.getType();
  if (type != term2.getType()) {
    return type < term2.getType() ? -1 : 1;
  }
  size_t len;
  int cmp;
  if (type == BNODE) {
    if (term1.bytes == NULL) {
      return term2.bytes == NULL ? 0 : -1;
    }
    if (term2.bytes == NULL) {
      return 1;
    }
  }
  if (term1.iri != NULL) {
    cmp = IRIRef::cmp(*term1.iri, *term2.iri);
    if (type == IRI || cmp != 0) {
      return cmp;
    }
  }
  if (type == LANG_LITERAL) {
    cmp = LangTag::cmp(*term1.lang, *term2.lang);
    if (cmp != 0) {
      return cmp;
    }
  }
  len = min(term1.bytes->size(), term2.bytes->size());
  cmp = memcmp(term1.bytes->dptr(), term2.bytes->dptr(),
               len * sizeof(uint8_t));
  if (cmp != 0) {
    return cmp;
  }
  return term1.bytes->size() < term2.bytes->size() ? -1 :
        (term1.bytes->size() > term2.bytes->size() ?  1 : 0);
}

DPtr<uint8_t> *RDFTerm::escape(DPtr<uint8_t> *str, bool as_iri)
    throw(SizeUnknownException, BadAllocException, BaseException<void*>) {
  if (str == NULL) {
    THROW(BaseException<void*>, NULL, "str must not be NULL.");
  }
  if (!str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint8_t *begin = str->dptr();
  const uint8_t *end = begin + str->size();
  const uint8_t *mark = begin;
  for (; mark != end &&
        *mark != to_ascii('\t') &&
        *mark != to_ascii('\n') &&
        *mark != to_ascii('\r') &&
        ( as_iri || *mark != to_ascii('"')) &&
        (!as_iri || *mark != to_ascii('>')) &&
        *mark != to_ascii('\\'); ++mark) {
    // loop finds first character that needs escaping, or end
  }
  if (mark == end) {
    str->hold();
    return str;
  }
  vector<uint8_t> vec;
  vec.reserve(str->size() + 1);
  vec.insert(vec.end(), begin, mark);
  do {
    vec.push_back(to_ascii('\\'));
    if (*mark == to_ascii('\t')) {
      vec.push_back(to_ascii('t'));
    } else if (*mark == to_ascii('\n')) {
      vec.push_back(to_ascii('n'));
    } else if (*mark == to_ascii('\r')) {
      vec.push_back(to_ascii('r'));
    } else if (*mark == to_ascii('"')) {
      if (as_iri) {
        vec.pop_back();
      } else {
        vec.push_back(to_ascii('"'));
      }
    } else if (*mark == to_ascii('>')) {
      vec.pop_back();
      vec.push_back(to_ascii('%'));
      vec.push_back(to_ascii('3'));
      vec.push_back(to_ascii('E'));
    } else if (*mark == to_ascii('\\')) {
      vec.push_back(to_ascii('\\'));
    }
    const uint8_t *begin = ++mark;
    for (; mark != end &&
          *mark != to_ascii('\t') &&
          *mark != to_ascii('\n') &&
          *mark != to_ascii('\r') &&
          ( as_iri || *mark != to_ascii('"')) &&
          (!as_iri || *mark != to_ascii('>')) &&
          *mark != to_ascii('\\'); ++mark) {
      // loop finds first character that needs escaping, or end
    }
    vec.insert(vec.end(), begin, mark);
  } while (mark != end);
  DPtr<uint8_t> *esc;
  try {
    NEW(esc, MPtr<uint8_t>, vec.size());
  } RETHROW_BAD_ALLOC
  copy(vec.begin(), vec.end(), esc->dptr());
  return esc;
}

DPtr<uint8_t> *RDFTerm::unescape(DPtr<uint8_t> *str, bool as_iri)
    throw(SizeUnknownException, BadAllocException, BaseException<void*>,
          TraceableException, InvalidEncodingException) {
  if (str == NULL) {
    THROW(BaseException<void*>, NULL, "str must not be NULL.");
  }
  if (!str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint8_t *begin = str->dptr();
  const uint8_t *end = begin + str->size();
  const uint8_t *mark = begin;
  for (; mark != end && *mark != to_ascii('\\'); ++mark) {
    // finds first escape sequence, or end
  }
  if (mark == end) {
    str->hold();
    return str;
  }
  vector<uint8_t> vec;
  vec.reserve(str->size() - 1);
  vec.insert(vec.end(), begin, mark);
  do {
    if (++mark == end) {
      THROW(TraceableException, "Escape sequence ended prematurely.");
    }
    if (*mark == to_ascii('\\')) {
      vec.push_back(*mark);
    } else if (!as_iri && *mark == to_ascii('"')) {
      vec.push_back(*mark);
    } else if (as_iri && *mark == to_ascii('>')) {
      vec.push_back(to_ascii('%'));
      vec.push_back(to_ascii('3'));
      vec.push_back(to_ascii('E'));
    } else if (*mark == to_ascii('n')) {
      vec.push_back(to_ascii('\n'));
    } else if (*mark == to_ascii('r')) {
      vec.push_back(to_ascii('\r'));
    } else if (*mark == to_ascii('t')) {
      vec.push_back(to_ascii('\t'));
    } else if (to_lower(*mark) == to_ascii('u')) {
      size_t i;
      size_t max;
      uint32_t c;
      c = UINT32_C(0x00);
      max = (*mark == to_ascii('u') ? 4 : 8);
      if (++mark == end) {
        THROW(TraceableException,
              "Unicode escape sequence ended prematurely.");
      }
      for (i = 0; i < max && mark != end; ++i) {
        c <<= 4;
        if (is_digit(*mark)) {
          c |= (*mark - to_ascii('0'));
        } else if (is_upper(*mark) && *mark <= to_ascii('F')) {
          c |= (*mark - to_ascii('A') + UINT32_C(10));
        } else if (is_lower(*mark) && *mark <= to_ascii('f')) {
          c |= (*mark - to_ascii('a') + UINT32_C(10));
        } else {
          THROW(TraceableException,
                "Syntactically invalid unicode escape sequence.");
        }
        ++mark;
      }
      --mark;
      if (i < max) {
        THROW(TraceableException,
              "Unicode escape sequence ended prematurely.");
      }
      uint8_t buf[4];
      try {
        max = utf8len(c, buf);
      } JUST_RETHROW (InvalidEncodingException,
          "Unicode escape sequence specifies invalid character.")
      vec.insert(vec.end(), buf, buf + max);
    } else {
      THROW(TraceableException, "Invalid escape sequence.");
    }
    for (++mark; mark != end && *mark != to_ascii('\\'); ++mark) {
      vec.push_back(*mark);
    }
  } while (mark != end);
  DPtr<uint8_t> *unesc;
  try {
    NEW(unesc, MPtr<uint8_t>, vec.size());
  } RETHROW_BAD_ALLOC
  copy(vec.begin(), vec.end(), unesc->dptr());
  return unesc;
}

RDFTerm RDFTerm::parse(DPtr<uint8_t> *utf8str)
    throw(SizeUnknownException, BaseException<void*>, TraceableException,
          InvalidEncodingException, InvalidCodepointException,
          MalformedIRIRefException, MalformedLangTagException,
          BaseException<IRIRef>) {
  if (utf8str == NULL) {
    THROW(BaseException<void*>, NULL, "utf8str must not be NULL.");
  }
  if (!utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  if (utf8str->size() < 2) {
    THROW(TraceableException, "Invalid RDFTerm string.");
  }
  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  if (*begin == to_ascii('<')) {
    if (*(end - 1) != to_ascii('>')) {
      THROW(TraceableException, "Encountered invalid RDFTerm IRI string.");
    }
    DPtr<uint8_t> *iristr = utf8str->sub(1, utf8str->size() - 2);
    DPtr<uint8_t> *unescaped = RDFTerm::unescape(iristr, true);
    iristr->drop();
    try {
      IRIRef iriref(unescaped);
      unescaped->drop();
      return RDFTerm(iriref);
    } catch (MalformedIRIRefException &e) {
      unescaped->drop();
      RETHROW(e, "Malformed IRI.");
    } catch (BaseException<IRIRef> &e) {
      // unescaped already dropped
      RETHROW(e, "Relative IRI.");
    }
  }
  if (*begin == to_ascii('_')) {
    if (*(begin + 1) != to_ascii(':')) {
      THROW(TraceableException,
            "Encountered invalid RDFTerm labelled BNODE string.");
    }
    try {
      utf8validate(utf8str);
    } JUST_RETHROW(InvalidEncodingException, "Expected UTF-8 encoded bnode.")
      JUST_RETHROW(InvalidCodepointException, "Invalid codepoint in bnode.")
    DPtr<uint8_t> *label = utf8str->sub(2, utf8str->size() - 2);
    RDFTerm term(label);
    label->drop();
    return term;
  }
  if (*begin == to_ascii('[')) {
    if (begin + 2 != end || *(begin + 1) != to_ascii(']')) {
      THROW(TraceableException,
            "Encountered invalid RDFTerm anonymous BNODE string.");
    }
    return RDFTerm();
  }
  if (*begin == to_ascii('"')) {
    const uint8_t *mark = end - 1;
    if (*mark != to_ascii('"')) {
      bool found = false;
      for (mark = begin + 1; mark != end; ++mark) {
        if (*mark == to_ascii('"')) {
          found = true;
          const uint8_t *backtrack = mark - 1;
          for (; *backtrack == to_ascii('\\'); --backtrack) {
            found = !found;
          }
          if (found) {
            break;
          }
        }
      }
      if (!found) {
        THROW(TraceableException,
              "Cannot find closing quotation mark in RDFTerm literal string.");
      }
    }
    DPtr<uint8_t> *lexp = utf8str->sub(1, mark - begin - 1);
    DPtr<uint8_t> *unesc_lex = RDFTerm::unescape(lexp, false);
    lexp->drop();
    try {
      utf8validate(unesc_lex);
    } catch (InvalidEncodingException &e) {
      unesc_lex->drop();
      RETHROW(e, "Lexical rep not UTF8 encoded correctly.");
    } catch (InvalidCodepointException &e) {
      unesc_lex->drop();
      RETHROW(e, "Lexical rep contains invalid codepoint.");
    }
    if (*(end - 1) == to_ascii('"')) {
      try {
        RDFTerm term(unesc_lex, NULL);
        unesc_lex->drop();
        return term;
      } catch (InvalidEncodingException &e) {
        unesc_lex->drop();
        RETHROW(e, "(rethrow)");
      } catch (InvalidCodepointException &e) {
        unesc_lex->drop();
        RETHROW(e, "(rethrow)");
      }
    }
    if (*(end - 1) == to_ascii('>')) {
      if (end - mark < 5 || mark[1] != to_ascii('^') ||
          mark[2] != to_ascii('^') || mark[3] != to_ascii('<')) {
        unesc_lex->drop();
        THROW(TraceableException, "Invalid RDFTerm TYPED_LITERAL string.");
      }
      DPtr<uint8_t> *dtp = utf8str->sub(mark - begin + 4, end - mark - 5);
      DPtr<uint8_t> *unesc_dt = RDFTerm::unescape(dtp, true);
      dtp->drop();
      try {
        IRIRef iriref(unesc_dt);
        unesc_dt->drop();
        try {
          RDFTerm term(unesc_lex, iriref);
          unesc_lex->drop();
          return term;
        } catch (InvalidEncodingException &e) {
          unesc_lex->drop();
          RETHROW(e, "(rethrow)");
        } catch (InvalidCodepointException &e) {
          unesc_lex->drop();
          RETHROW(e, "(rethrow)");
        }
      } catch (MalformedIRIRefException &e) {
        unesc_dt->drop();
        unesc_lex->drop();
        RETHROW(e, "Malformed datatype IRI.");
      }
    }
    if (end - mark < 2 || mark[1] != to_ascii('@')) {
      unesc_lex->drop();
      THROW(TraceableException,
            "Invalid tag following lexical form in RDFTerm literal string.");
    }
    DPtr<uint8_t> *langp = utf8str->sub(mark - begin + 2, end - mark - 2);
    try {
      LangTag lang(langp);
      langp->drop();
      try {
        RDFTerm term(unesc_lex, &lang);
        unesc_lex->drop();
        return term;
      } catch (InvalidEncodingException &e) {
        unesc_lex->drop();
        RETHROW(e, "(rethrow)");
      } catch (InvalidCodepointException &e) {
        unesc_lex->drop();
        RETHROW(e, "(rethrow)");
      }
    } catch (MalformedLangTagException &e) {
      unesc_lex->drop();
      RETHROW(e, "Invalid literal language tag.");
    }
  }
  THROW(TraceableException, "Invalid RDFTerm string.");
}

DPtr<uint8_t> *RDFTerm::toUTF8String() const throw(BadAllocException) {
  switch (this->type) {
  case BNODE: {
    DPtr<uint8_t> *str;
    if (this->bytes == NULL) {
      try {
        NEW(str, MPtr<uint8_t>, 2);
      } RETHROW_BAD_ALLOC
      ascii_strcpy(str->dptr(), "[]");
      return str;
    }
    try {
      NEW(str, MPtr<uint8_t>, this->bytes->size() + 2);
    } RETHROW_BAD_ALLOC
    ascii_strcpy(str->dptr(), "_:");
    memcpy(str->dptr() + 2, this->bytes->dptr(),
           this->bytes->size() * sizeof(uint8_t));
    return str;
  }
  case IRI: {
    DPtr<uint8_t> *iristr = this->iri->getUTF8String();
    DPtr<uint8_t> *escaped = RDFTerm::escape(iristr, true);
    iristr->drop();
    DPtr<uint8_t> *str;
    try {
      NEW(str, MPtr<uint8_t>, escaped->size() + 2);
    } RETHROW_BAD_ALLOC
    (*str)[0] = to_ascii('<');
    memcpy(str->dptr() + 1, escaped->dptr(),
           escaped->size() * sizeof(uint8_t));
    (*str)[str->size() - 1] = to_ascii('>');
    escaped->drop();
    return str;
  }
  case SIMPLE_LITERAL: {
    DPtr<uint8_t> *escaped = RDFTerm::escape(this->bytes, false);
    DPtr<uint8_t> *str;
    try {
      NEW(str, MPtr<uint8_t>, escaped->size() + 2);
    } RETHROW_BAD_ALLOC
    (*str)[0] = to_ascii('"');
    memcpy(str->dptr() + 1, escaped->dptr(),
           escaped->size() * sizeof(uint8_t));
    (*str)[str->size() - 1] = to_ascii('"');
    escaped->drop();
    return str;
  }
  case LANG_LITERAL: {
    DPtr<uint8_t> *escaped = RDFTerm::escape(this->bytes, false);
    DPtr<uint8_t> *langstr = this->lang->getASCIIString();
    DPtr<uint8_t> *str;
    try {
      NEW(str, MPtr<uint8_t>, escaped->size() + langstr->size() + 3);
    } RETHROW_BAD_ALLOC
    (*str)[0] = to_ascii('"');
    memcpy(str->dptr() + 1, escaped->dptr(),
           escaped->size() * sizeof(uint8_t));
    ascii_strcpy(str->dptr() + escaped->size() + 1, "\"@");
    memcpy(str->dptr() + escaped->size() + 3, langstr->dptr(),
           langstr->size() * sizeof(uint8_t));
    escaped->drop();
    langstr->drop();
    return str;
  }
  case TYPED_LITERAL: {
    DPtr<uint8_t> *escaped = RDFTerm::escape(this->bytes, false);
    DPtr<uint8_t> *dtstr = this->iri->getUTF8String();
    DPtr<uint8_t> *str;
    try {
      NEW(str, MPtr<uint8_t>, escaped->size() + dtstr->size() + 6);
    } RETHROW_BAD_ALLOC
    (*str)[0] = to_ascii('"');
    memcpy(str->dptr() + 1, escaped->dptr(),
           escaped->size() * sizeof(uint8_t));
    ascii_strcpy(str->dptr() + escaped->size() + 1, "\"^^<");
    memcpy(str->dptr() + escaped->size() + 5, dtstr->dptr(),
           dtstr->size() * sizeof(uint8_t));
    (*str)[str->size() - 1] = to_ascii('>');
    escaped->drop();
    dtstr->drop();
    return str;
  }
  }
}

RDFTerm &RDFTerm::normalize() throw(BadAllocException, TraceableException) {
  if (this->normalized) {
    return *this;
  }
  if (this->iri != NULL) {
    try {
      this->iri->normalize();
    } RETHROW_BAD_ALLOC
  }
  if (this->lang != NULL) {
    try {
      this->lang->normalize();
    } RETHROW_BAD_ALLOC
  }
  if (this->type != BNODE && this->bytes != NULL) {
    const uint8_t *begin = this->bytes->dptr();
    const uint8_t *end = begin + this->bytes->size();
    for (; begin != end && is_ascii(*begin); ++begin) {
      // see if all ASCII
    }
    // if all ASCII, don't bother with NFC normalization
    if (begin != end) {
      DPtr<uint32_t> *codepoints;
      try {
        codepoints = utf8dec(this->bytes);
      } JUST_RETHROW(BadAllocException, "(rethrow)")
      DPtr<uint32_t> *nfcnorm;
      try {
        nfcnorm = nfc_opt(codepoints);
      } catch (BadAllocException &e) {
        codepoints->drop();
        RETHROW(e, "(rethrow)");
      }
      codepoints->drop();
      DPtr<uint8_t> *nfcbytes;
      try {
        nfcbytes = utf8enc(nfcnorm);
      } catch (BadAllocException &e) {
        nfcnorm->drop();
        RETHROW(e, "(rethrow)");
      }
      nfcnorm->drop();
      this->bytes->drop();
      this->bytes = nfcbytes;
    }
  }
  this->normalized = true;
  return *this;
}

DPtr<uint8_t> *RDFTerm::getLabel() const
    throw(BaseException<enum RDFTermType>) {
  if (this->type != BNODE) {
    THROW(BaseException<enum RDFTermType>, this->type,
        "Invalid call to getLabel() for this RDFTermType.");
  }
  if (this->bytes != NULL) {
    this->bytes->hold();
  }
  return this->bytes;
}

IRIRef RDFTerm::getIRIRef() const throw(BaseException<enum RDFTermType>) {
  if (this->type != IRI) {
    THROW(BaseException<enum RDFTermType>, this->type,
        "Invalid call to getIRIRef() for this RDFTermType.");
  }
  return *(this->iri);
}

DPtr<uint8_t> *RDFTerm::getLexForm() const
    throw(BaseException<enum RDFTermType>) {
  if (!this->isLiteral()) {
    THROW(BaseException<enum RDFTermType>, this->type,
        "Invalid call to getLexForm() for this RDFTermType.");
  }
  this->bytes->hold();
  return this->bytes;
}

LangTag RDFTerm::getLangTag() const throw(BaseException<enum RDFTermType>) {
  if (this->type != LANG_LITERAL) {
    THROW(BaseException<enum RDFTermType>, this->type,
        "Invalid call to getLangTag() for this RDFTermType.");
  }
  return *(this->lang);
}

IRIRef RDFTerm::getDatatype() const throw(BaseException<enum RDFTermType>) {
  if (this->type != TYPED_LITERAL) {
    THROW(BaseException<enum RDFTermType>, this->type,
        "Invalid call to getDatatype() for this RDFTermType.");
  }
  return *(this->iri);
}

RDFTerm &RDFTerm::operator=(const RDFTerm &rhs) throw(BadAllocException) {
  if (this == &rhs) {
    return *this;
  }
  if (this->iri == NULL) {
    if (rhs.iri != NULL) {
      try {
        NEW(this->iri, IRIRef, *(rhs.iri));
      } RETHROW_BAD_ALLOC
    }
  } else if (rhs.iri != NULL) {
    *(this->iri) = *(rhs.iri);
  } else {
    DELETE(this->iri);
    this->iri = NULL;
  }
  if (this->lang == NULL) {
    if (rhs.lang != NULL) {
      try {
        NEW(this->lang, LangTag, *(rhs.lang));
      } RETHROW_BAD_ALLOC
    }
  } else if (rhs.lang != NULL) {
    *(this->lang) = *(rhs.lang);
  } else {
    DELETE(this->lang);
    this->lang = NULL;
  }
  if (this->bytes != NULL) {
    this->bytes->drop();
  }
  this->bytes = rhs.bytes;
  if (this->bytes != NULL) {
    this->bytes->hold();
  }
  this->type = rhs.type;
  this->normalized = rhs.normalized;
}

} // end namespace rdf

std::ostream& operator<<(std::ostream &stream, const rdf::RDFTerm &term) {
  ptr::DPtr<uint8_t> *utf8str = term.toUTF8String();
  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  for (; begin != end; ++begin) {
    stream << to_lchar(*begin);
  }
  utf8str->drop();
  return stream;
}
