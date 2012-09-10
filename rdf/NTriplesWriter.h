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

#ifndef __RDF__NTRIPLESWRITER_H__
#define __RDF__NTRIPLESWRITER_H__

#include "ex/BaseException.h"
#include "io/OutputStream.h"
#include "ptr/BadAllocException.h"
#include "rdf/RDFWriter.h"
#include "sys/ints.h"
#include "ucs/InvalidEncodingException.h"

namespace rdf {

using namespace ex;
using namespace io;
using namespace ptr;
using namespace std;
using namespace ucs;

class NTriplesWriter : public RDFWriter {
private:
  OutputStream *output;
public:
  NTriplesWriter(OutputStream *os) throw(BaseException<void*>);
  ~NTriplesWriter() throw();

  static DPtr<uint8_t> *escapeUnicode(DPtr<uint8_t> *utf8str)
      throw(BadAllocException, InvalidEncodingException);
  static RDFTerm sanitize(const RDFTerm &bnode)
      throw(BadAllocException);

  void write(const RDFTriple &triple);
  void close();
};

}

#endif /* __RDF__NTRIPLESWRITER_H__ */
