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

#include "rdf/RDFDictionary.h"

#include <iostream>
#include "ex/TraceableException.h"

namespace rdf {

using namespace ex;
using namespace std;

template<typename ID, typename ENC>
RDFDictionary<ID, ENC>::RDFDictionary() throw()
    : term2id(Term2IDMap(RDFTerm::cmplt0)), counter(ID(1)) {
  // do nothing
}

template<typename ID, typename ENC>
RDFDictionary<ID, ENC>::RDFDictionary(const ID &init) throw()
    : term2id(Term2IDMap(RDFTerm::cmplt0)), counter(init) {
  // do nothing
}

template<typename ID, typename ENC>
RDFDictionary<ID, ENC>::RDFDictionary(const ENC &enc) throw()
    : term2id(Term2IDMap(RDFTerm::cmplt0)), counter(ID(1)),
      encoder(enc) {
  // do nothing
}

template<typename ID, typename ENC>
RDFDictionary<ID, ENC>::RDFDictionary(const ID &init, const ENC &enc) throw()
    : term2id(Term2IDMap(RDFTerm::cmplt0)), counter(init), encoder(enc) {
  // do nothing
}

template<typename ID, typename ENC>
RDFDictionary<ID, ENC>::~RDFDictionary() throw() {
  // do nothing
}

template<typename ID, typename ENC>
bool RDFDictionary<ID, ENC>::nextID(ID &id) {
  if (this->counter[(ID::size() << 3) - 1]) {
    return false;
  }
  id = this->counter;
  ++this->counter;
  return true;
}

template<typename ID, typename ENC>
ID RDFDictionary<ID, ENC>::encode(const RDFTerm &term) {
  pair<typename Term2IDMap::iterator, typename Term2IDMap::iterator> range =
      this->term2id.equal_range(term);
  if (range.first != range.second) {
    return range.first->second;
  }
  ID id;
  if (!this->nextID(id)) {
    THROW(TraceableException, "Ran out of identifiers!");
  }
  if (range.first != this->term2id.begin()) {
    --range.first;
  }

  this->term2id.insert(range.first, pair<RDFTerm, ID>(term, id));

  typename ID2TermMap::iterator it = this->id2term.end();
  if (it != this->id2term.begin()) {
    --it;
  }
  this->id2term.insert(it, pair<ID, RDFTerm>(id, term));

  return id;
}

template<typename ID, typename ENC>
RDFTerm RDFDictionary<ID, ENC>::decode(const ID &id) {
  RDFTerm term;
  if (this->lookup(id, term)) {
    return term;
  }
  THROW(TraceableException, "Cannot decode term!");
}

template<typename ID, typename ENC>
bool RDFDictionary<ID, ENC>::lookup(const RDFTerm &term) {
  ID id;
  return this->lookup(term, id);
}

template<typename ID, typename ENC>
bool RDFDictionary<ID, ENC>::lookup(const RDFTerm &term, ID &id) {
  if (this->encoder(term, id) && !id((ID::size() << 3) - 1, true)) {
    return true;
  }
  typename Term2IDMap::const_iterator it = this->term2id.find(term);
  if (it != this->term2id.end()) {
    id = it->second;
    return true;
  }
  return false;
}

template<typename ID, typename ENC>
bool RDFDictionary<ID, ENC>::lookup(const ID &id) {
  RDFTerm term;
  return this->lookup(id, term);
}

template<typename ID, typename ENC>
bool RDFDictionary<ID, ENC>::lookup(const ID &id, RDFTerm &term) {
  ID myid = id;
  if (myid((ID::size() << 3) - 1, false)) {
    return this->encoder(myid, term);
  }
  typename ID2TermMap::const_iterator it = this->id2term.find(myid);
  if (it != this->id2term.end()) {
    term = it->second;
    return true;
  }
  return false;
}

}
