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

#ifndef __RDF__RDFDICTIONARY_H__
#define __RDF__RDFDICTIONARY_H__

#include <map>
#include "rdf/RDFEncoder.h"
#include "rdf/RDFTerm.h"
#include "sys/ints.h"

namespace rdf {

using namespace std;

template<typename ID=RDFID<8>, typename ENC=RDFEncoder<ID> >
class RDFDictionary {
protected:
  typedef map<RDFTerm, ID, bool(*)(const RDFTerm &, const RDFTerm &)>
      Term2IDMap;
  typedef map<ID, RDFTerm> ID2TermMap;
  Term2IDMap term2id;
  ID2TermMap id2term;
  ID counter;
  ENC encoder;
  virtual bool nextID(ID &id);
  RDFDictionary(const ID &init) throw();
  RDFDictionary(const ID &init, const ENC &enc) throw();
public:
  RDFDictionary() throw();
  RDFDictionary(const ENC &enc) throw();
  virtual ~RDFDictionary() throw();

  virtual ID encode(const RDFTerm &term);
  virtual RDFTerm decode(const ID &id);
  virtual bool lookup(const RDFTerm &term);
  virtual bool lookup(const RDFTerm &term, ID &id);
  virtual bool lookup(const ID &id);
  virtual bool lookup(const ID &id, RDFTerm &term);
};

}

#include "rdf/RDFDictionary-inl.h"

#endif /* __RDF__RDFDICTIONARY_H__ */
