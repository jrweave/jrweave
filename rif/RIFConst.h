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

#ifndef __RIF__RIFCONST_H__
#define __RIF__RIFCONST_H__

#include "ex/BaseException.h"
#include "iri/IRIRef.h"
#include "ptr/DPtr.h"
#include "ucs/InvalidCodepointException.h"
#include "ucs/InvalidEncodingException.h"

namespace rif {

using namespace ex;
using namespace iri;
using namespace ptr;
using namespace std;
using namespace ucs;

// Considered as a strictly syntactic structure; no lexical validation.
class RIFConst {
private:
  DPtr<uint8_t> *lex;
  IRIRef datatype;
  bool normalized;
public:
  RIFConst(DPtr<uint8_t> *utf8str, const IRIRef &dt)
      throw(SizeUnknownException, BaseException<void*>, BaseException<IRIRef>,
            InvalidCodepointException, InvalidEncodingException);
  RIFConst(const RIFConst &copy) throw();
  ~RIFConst() throw();

  // Static Methods
  static int cmp(const RIFConst &rc1, const RIFConst &rc2) throw();
  static bool cmplt0(const RIFConst &rc1, const RIFConst &rc2) throw();
  static bool cmpeq0(const RIFConst &rc1, const RIFConst &rc2) throw();
  static DPtr<uint8_t> *escape(DPtr<uint8_t> *lex)
      throw(SizeUnknownException, BadAllocException, BaseException<void*>);
  static DPtr<uint8_t> *unescape(DPtr<uint8_t> *lex)
      throw(SizeUnknownException, BadAllocException, BaseException<void*>,
            TraceableException);
  static RIFConst parse(DPtr<uint8_t> *utf8str)
      throw(BadAllocException, SizeUnknownException,
            InvalidCodepointException, InvalidEncodingException,
            TraceableException, MalformedIRIRefException);

  bool equals(const RIFConst &rhs) const throw();
  RIFConst &normalize() throw(BadAllocException);
  DPtr<uint8_t> *getLexForm() const throw();
  IRIRef getDatatype() const throw();
  DPtr<uint8_t> *toUTF8String() const throw(BadAllocException);

  RIFConst &operator=(const RIFConst &rhs) throw();
};

}

#include "rif/RIFConst-inl.h"

#endif /* __RIF__RIFCONST_H__ */
