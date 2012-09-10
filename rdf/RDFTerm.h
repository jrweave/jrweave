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

#ifndef __RDF__RDFTERM_H__
#define __RDF__RDFTERM_H__

#include <iostream>
#include "ex/BaseException.h"
#include "iri/IRIRef.h"
#include "lang/LangTag.h"
#include "ptr/DPtr.h"
#include "ptr/SizeUnknownException.h"
#include "sys/ints.h"
#include "ucs/InvalidCodepointException.h"
#include "ucs/InvalidEncodingException.h"

namespace rdf {

using namespace ex;
using namespace iri;
using namespace lang;
using namespace ptr;
using namespace std;
using namespace ucs;

enum RDFTermType {
  BNODE = 1,
  IRI = 2,
  SIMPLE_LITERAL = 3,
  LANG_LITERAL = 4,
  TYPED_LITERAL = 5
};

// Considered here solely as a SYNTACTIC entity;
// i.e., no interpretation, scoping, etc.
class RDFTerm {
private:
  IRIRef *iri;
  LangTag *lang;
  DPtr<uint8_t> *bytes;
  enum RDFTermType type;
  bool normalized;
public:
  RDFTerm() throw(); // anonymous blank node
  RDFTerm(DPtr<uint8_t> *label) throw(SizeUnknownException);
  RDFTerm(const IRIRef &iriref)
      throw(BaseException<IRIRef>, BadAllocException);
  RDFTerm(DPtr<uint8_t> *utf8lex, const LangTag *lang)
      throw(SizeUnknownException, BaseException<void*>, BadAllocException,
            InvalidEncodingException, InvalidCodepointException);
  RDFTerm(DPtr<uint8_t> *utf8lex, const IRIRef &datatype)
      throw(SizeUnknownException, BaseException<void*>, BadAllocException,
            InvalidEncodingException, InvalidCodepointException);
  RDFTerm(const RDFTerm &copy) throw(BadAllocException);
  ~RDFTerm() throw();

  // Static Methods
  static int cmp(const RDFTerm &term1, const RDFTerm &term2) throw();
  static bool cmplt0(const RDFTerm &term1, const RDFTerm &term2) throw();
  static bool cmpeq0(const RDFTerm &term1, const RDFTerm &term2) throw();
  static DPtr<uint8_t> *escape(DPtr<uint8_t> *str, bool as_iri)
      throw(SizeUnknownException, BadAllocException, BaseException<void*>);
  static DPtr<uint8_t> *unescape(DPtr<uint8_t> *str, bool as_iri)
      throw(SizeUnknownException, BadAllocException, BaseException<void*>,
            InvalidEncodingException, TraceableException);
  static RDFTerm parse(DPtr<uint8_t> *utf8str)
      throw(SizeUnknownException, BaseException<void*>, TraceableException,
            InvalidEncodingException, InvalidCodepointException,
            MalformedIRIRefException, MalformedLangTagException,
            BaseException<IRIRef>);

  // Final Methods
  enum RDFTermType getType() const throw();
  bool isAnonymous() const throw();
  bool isLiteral() const throw();
  bool isPlainLiteral() const throw();
  DPtr<uint8_t> *toUTF8String() const throw(BadAllocException);
  bool equals(const RDFTerm &term) const throw();
  RDFTerm &normalize() throw(BadAllocException, TraceableException);

  // BNODE
  DPtr<uint8_t> *getLabel() const throw(BaseException<enum RDFTermType>);

  // IRI
  IRIRef getIRIRef() const throw(BaseException<enum RDFTermType>);

  // SIMPLE_LITERAL, LANG_LITERAL, and TYPED_LITERAL
  DPtr<uint8_t> *getLexForm() const throw(BaseException<enum RDFTermType>);

  // LANG_LITERAL
  LangTag getLangTag() const throw(BaseException<enum RDFTermType>);

  // TYPED_LITERAL
  IRIRef getDatatype() const throw(BaseException<enum RDFTermType>);

  RDFTerm &operator=(const RDFTerm &rhs) throw(BadAllocException);
};

} // end namespace rdf

// TODO
//std::istream& operator>>(std::istream &stream, rdf::RDFTerm &term);
std::ostream& operator<<(std::ostream &stream, const rdf::RDFTerm &term);

#include "rdf/RDFTerm-inl.h"

#endif /* __RDF__RDFTERM_H__ */
