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

inline
RDFTriple::RDFTriple() throw() {
  // do nothing
}

inline
RDFTriple::~RDFTriple() throw() {
  // do nothing
}

inline
bool RDFTriple::cmplt0(const RDFTriple &t1, const RDFTriple &t2) throw() {
  return RDFTriple::cmp(t1, t2) < 0;
}

inline
bool RDFTriple::cmpeq0(const RDFTriple &t1, const RDFTriple &t2) throw() {
  return RDFTriple::cmp(t1, t2) == 0;
}

inline
DPtr<uint8_t> *RDFTriple::toUTF8String() const THROWS(BadAllocException) {
  return this->toUTF8String(false);
}
TRACE(BadAllocException, "(trace)")

inline
bool RDFTriple::equals(const RDFTriple &t) const throw() {
  return RDFTriple::cmp(*this, t) == 0;
}

inline
enum RDFTermType RDFTriple::getSubjType() const throw() {
  return this->subject.getType();
}

inline
RDFTerm RDFTriple::getSubj() const throw() {
  return this->subject;
}

inline
IRIRef RDFTriple::getSubjIRI() const THROWS(BaseException<enum RDFTermType>) {
  return this->subject.getIRIRef();
}
TRACE(BaseException<enum RDFTermType>, "(trace)")

inline
RDFTerm RDFTriple::getPred() const throw() {
  return this->predicate;
}

inline
IRIRef RDFTriple::getPredIRI() const throw() {
  return this->predicate.getIRIRef();
}

inline
enum RDFTermType RDFTriple::getObjType() const throw() {
  return this->object.getType();
}

inline
RDFTerm RDFTriple::getObj() const throw() {
  return this->object;
}

inline
IRIRef RDFTriple::getObjIRI() const THROWS(BaseException<enum RDFTermType>) {
  return this->object.getIRIRef();
}
TRACE(BaseException<enum RDFTermType>, "(trace)")

}
