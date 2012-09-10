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

#include "rdf/RDFTriple.h"

namespace rdf {

RDFTriple::RDFTriple(const RDFTerm &subj, const RDFTerm &pred,
                     const RDFTerm &obj)
    THROWS(BaseException<enum RDFTermType>, BadAllocException) {
  if (subj.isLiteral()) {
    THROW(BaseException<enum RDFTermType>, subj.getType(),
          "Subject must not be a literal.");
  }
  if (pred.getType() != IRI) {
    THROW(BaseException<enum RDFTermType>, pred.getType(),
          "Predicate must be an IRI.");
  }
  this->subject = subj;
  this->predicate = pred;
  this->object = obj;
}
TRACE(BadAllocException, "(trace)")

RDFTriple::RDFTriple(const RDFTriple &copy) THROWS(BadAllocException) {
  this->subject = copy.subject;
  this->predicate = copy.predicate;
  this->object = copy.object;
}
TRACE(BadAllocException, "(trace)")

int RDFTriple::cmp(const RDFTriple &t1, const RDFTriple &t2) throw() {
  int c = RDFTerm::cmp(t1.subject, t2.subject);
  if (c != 0) {
    return c;
  }
  c = RDFTerm::cmp(t1.predicate, t2.predicate);
  if (c != 0) {
    return c;
  }
  return RDFTerm::cmp(t1.object, t2.object);
}

RDFTriple RDFTriple::parse(DPtr<uint8_t> *utf8str)
    THROWS(SizeUnknownException, BadAllocException, TraceableException,
           InvalidEncodingException, InvalidCodepointException,
           MalformedIRIRefException, MalformedLangTagException,
           BaseException<IRIRef>) {
  if (utf8str == NULL || !utf8str->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  for (; begin != end && is_space(*begin); ++begin) {
    // find first non-space, or end
  }
  if (begin == end) {
    THROW(TraceableException, "Invalid triple; could not find subject.");
  }
  const uint8_t *mark = begin;
  for (; mark != end && !is_space(*mark); ++mark) {
    // find end of subject
  }
  if (mark == end) {
    THROW(TraceableException, "Invalid triple; only a subject.");
  }
  DPtr<uint8_t> *part = utf8str->sub(begin - utf8str->dptr(),
                                     mark - begin);
  RDFTerm subject;
  try {
    subject = RDFTerm::parse(part);
  } catch (InvalidEncodingException &e) {
    part->drop();
    RETHROW(e, "Invalid UTF-8 encoding of subject.");
  } catch (InvalidCodepointException &e) {
    part->drop();
    RETHROW(e, "Invalid codepoint in subject.");
  } catch (MalformedIRIRefException &e) {
    part->drop();
    RETHROW(e, "Subject is malformed IRI.");
  } catch (MalformedLangTagException &e) {
    part->drop();
    RETHROW(e,
        "Subject has malformed LangTag (shouldn't have LangTag anyway).");
  } catch (BaseException<IRIRef> &e) {
    part->drop();
    RETHROW(e, "Problem parsing subject.");
  } catch (TraceableException &e) {
    part->drop();
    RETHROW(e, "Problem parsing subject.");
  }
  part->drop();

  for (begin = mark; begin != end && is_space(*begin); ++begin) {
    // find beginning of predicate
  }
  if (begin == end) {
    THROW(TraceableException, "Invalid triple, could not find predicate.");
  }
  for (mark = begin; mark != end && !is_space(*mark); ++mark) {
    // find end of predicate
  }
  if (mark == end) {
    THROW(TraceableException, "Invalid triple; ended after predicate.");
  }
  part = utf8str->sub(begin - utf8str->dptr(), mark - begin);
  RDFTerm predicate;
  try {
    predicate = RDFTerm::parse(part);
  } catch (InvalidEncodingException &e) {
    part->drop();
    RETHROW(e, "Invalid UTF-8 encoding of predicate.");
  } catch (InvalidCodepointException &e) {
    part->drop();
    RETHROW(e, "Invalid codepoint in predicate.");
  } catch (MalformedIRIRefException &e) {
    part->drop();
    RETHROW(e, "Predicate is malformed IRI.");
  } catch (MalformedLangTagException &e) {
    part->drop();
    RETHROW(e,
         "Predicate has malformed LangTag (shouldn't have LangTag anyway).");
  } catch (BaseException<IRIRef> &e) {
    part->drop();
    RETHROW(e, "Problem parsing predicate.");
  } catch (TraceableException &e) {
    part->drop();
    RETHROW(e, "Problem parsing predicate.");
  }
  part->drop();
  
  for (begin = mark; begin != end && is_space(*begin); ++begin) {
    // find beginning of object
  }
  if (begin == end) {
    THROW(TraceableException, "Invalid triple; no object.");
  }
  for (mark = end - 1; mark != begin && is_space(*mark); --mark) {
    // find end of triple, optional '.'
  }
  if (*mark == to_ascii('.') && mark != begin) {
    for (--mark; mark != begin && is_space(*mark); --mark) {
      // find end of object
    }
  }
  ++mark;
  part = utf8str->sub(begin - utf8str->dptr(), mark - begin);
  RDFTerm object;
  try {
    object = RDFTerm::parse(part);
  } catch (InvalidEncodingException &e) {
    part->drop();
    RETHROW(e, "Invalid UTF-8 encoding of object.");
  } catch (InvalidCodepointException &e) {
    part->drop();
    RETHROW(e, "Invalid codepoint in object.");
  } catch (MalformedIRIRefException &e) {
    part->drop();
    RETHROW(e, "Object is malformed IRI.");
  } catch (MalformedLangTagException &e) {
    part->drop();
    RETHROW(e, "Object has malformed LangTag.");
  } catch (BaseException<IRIRef> &e) {
    part->drop();
    RETHROW(e, "Problem parsing object.");
  } catch (TraceableException &e) {
    part->drop();
    RETHROW(e, "Problem parsing object.");
  }
  part->drop();
  
  return RDFTriple(subject, predicate, object);
}
TRACE(BadAllocException, "(trace)")

DPtr<uint8_t> *RDFTriple::toUTF8String(const bool with_dot_endl) const
    throw(BadAllocException) {
  DPtr<uint8_t> *sstr, *pstr, *ostr;
  try {
    sstr = this->subject.toUTF8String();
  } JUST_RETHROW(BadAllocException, "(rethrow)")
  try {
    pstr = this->predicate.toUTF8String();
  } catch (BadAllocException &e) {
    sstr->drop();
    RETHROW(e, "(rethrow)");
  }
  try {
    ostr = this->object.toUTF8String();
  } catch (BadAllocException &e) {
    sstr->drop();
    pstr->drop();
    RETHROW(e, "(rethrow)");
  }
  size_t len = sstr->size() + pstr->size() + ostr->size() + 2;
  if (with_dot_endl) {
    len += 3;
  }
  DPtr<uint8_t> *str;
  try {
    NEW(str, MPtr<uint8_t>, len);
  } catch (bad_alloc &e) {
    sstr->drop();
    pstr->drop();
    ostr->drop();
    THROW(BadAllocException, sizeof(MPtr<uint8_t>));
  } catch (BadAllocException &e) {
    sstr->drop();
    pstr->drop();
    ostr->drop();
    RETHROW(e, "(rethrow)");
  }
  uint8_t *p = str->dptr();
  memcpy(p, sstr->dptr(), sstr->size() * sizeof(uint8_t));
  p += sstr->size();
  *p = to_ascii(' ');
  ++p;
  sstr->drop();
  memcpy(p, pstr->dptr(), pstr->size() * sizeof(uint8_t));
  p += pstr->size();
  *p = to_ascii(' ');
  ++p;
  pstr->drop();
  memcpy(p, ostr->dptr(), ostr->size() * sizeof(uint8_t));
  if (with_dot_endl) {
    p += ostr->size();
    ascii_strcpy(p, " .\n");
  }
  ostr->drop();
  return str;
}

RDFTriple &RDFTriple::normalize() THROWS(BadAllocException,
    TraceableException) {
  this->subject.normalize();
  this->predicate.normalize();
  this->object.normalize();
  return *this;
}
TRACE(BadAllocException, "(trace)")

RDFTriple &RDFTriple::operator=(const RDFTriple &rhs) 
    THROWS(BadAllocException) {
  this->subject = rhs.subject;
  this->predicate = rhs.predicate;
  this->object = rhs.object;
  return *this;
}
TRACE(BadAllocException, "(trace)")

}

std::ostream& operator<<(std::ostream &stream, const rdf::RDFTriple &triple) {
  stream << triple.getSubj() << " "
         << triple.getPred() << " "
         << triple.getObj() << " .\n";
  return stream;
}
