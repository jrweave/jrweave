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

#ifndef __IRI__IRIREF_H__
#define __IRI__IRIREF_H__

#include <iostream>
#include <vector>
#include "iri/MalformedIRIRefException.h"
#include "ptr/BadAllocException.h"
#include "ptr/DPtr.h"
#include "sys/char.h"
#include "sys/ints.h"
#include "ucs/UCSIter.h"
#include "ucs/InvalidCodepointException.h"
#include "ucs/InvalidEncodingException.h"

#define IRI_HEX_VALUE(c) (is_digit(c) ? (c) - to_ascii('0') : (is_alpha(c) ? to_upper(c) - to_ascii('A') + 10 : UINT32_C(16)))

namespace iri {

using namespace ptr;
using namespace std;
using namespace ucs;

template<typename iter>
bool isIRIReference(iter begin, iter end);

template<typename iter>
bool isIRI(iter begin, iter end);

template<typename iter>
bool isIRelativeRef(iter begin, iter end);

template<typename iter>
bool isScheme(iter begin, iter end);

template<typename iter>
bool isIHierPart(iter begin, iter end);

template<typename iter>
bool isIRelativePart(iter begin, iter end);

template<typename iter>
bool isIAuthority(iter begin, iter end);

template<typename iter>
bool isIUserInfo(iter begin, iter end);

template<typename iter>
bool isIHost(iter begin, iter end);

template<typename iter>
bool isIPLiteral(iter begin, iter end);

template<typename iter>
bool isIPv4Address(iter begin, iter end);

template<typename iter>
bool isIPv6Address(iter begin, iter end);

template<typename iter>
bool isIPvFutureAddress(iter begin, iter end);

template<typename iter>
bool isIRegName(iter begin, iter end);

template<typename iter>
bool isPort(iter begin, iter end);

template<typename iter>
bool isIPath(iter begin, iter end);

template<typename iter>
bool isIPathAbsEmpty(iter begin, iter end);

template<typename iter>
bool isIPathAbsolute(iter begin, iter end);

template<typename iter>
bool isIPathNoScheme(iter begin, iter end);

template<typename iter>
bool isIPathRootless(iter begin, iter end);

template<typename iter>
bool isIPathEmpty(iter begin, iter end);

template<typename iter>
bool isISegment(iter begin, iter end);

template<typename iter>
bool isISegmentNZ(iter begin, iter end);

template<typename iter>
bool isISegmentNZNC(iter begin, iter end);

template<typename iter>
bool isIQuery(iter begin, iter end);

template<typename iter>
bool isIFragment(iter begin, iter end);

template<typename iter>
bool isPctEncoded(iter begin, iter end);

class IRIRef;

enum IRIRefPart {
  SCHEME, USER_INFO, HOST, PORT, PATH, QUERY, FRAGMENT
};

class IRIRef {
protected:
  DPtr<uint8_t> *utf8str;
  bool normalized;
  bool urified;
public:
  IRIRef() throw(BadAllocException);
      // <> relative IRI reference with empty path
  IRIRef(DPtr<uint8_t> *utf8str)
      throw(SizeUnknownException, MalformedIRIRefException,
            InvalidEncodingException, InvalidCodepointException);
  IRIRef(const IRIRef &iri) throw();
  IRIRef(const IRIRef *iri) throw();
  virtual ~IRIRef() throw();

  // Static Methods
  static int cmp(const IRIRef &ref1, const IRIRef &ref2) throw();
  static bool cmplt0(const IRIRef &ref1, const IRIRef &ref2) throw();
  static bool cmpeq0(const IRIRef &ref1, const IRIRef &ref2) throw();
  static bool isIPChar(const uint32_t codepoint) throw();
  static bool isReserved(const uint32_t codepoint) throw();
  static bool isUnreserved(const uint32_t codepoint) throw();
  static bool isIUnreserved(const uint32_t codepoint) throw();
  static bool isUCSChar(const uint32_t codepoint) throw();
  static bool isSubDelim(const uint32_t codepoint) throw();
  static bool isGenDelim(const uint32_t codepoint) throw();
  static bool isIPrivate(const uint32_t codepoint) throw();

  // Final Methods
  DPtr<uint8_t> *getUTF8String() const throw();
  bool equals(const IRIRef &ref) const throw();

  bool isRelativeRef() const throw();
  bool isIRI() const throw();
  bool isAbsoluteIRI() const throw();

  DPtr<uint8_t> *getPart(const enum IRIRefPart part) const throw();

  // Virtual Methods
  virtual IRIRef *normalize() throw(BadAllocException, TraceableException);
  virtual IRIRef *urify() throw(BadAllocException);
  virtual IRIRef *resolve(IRIRef *base) throw(BadAllocException);

  // Operators
  IRIRef &operator=(const IRIRef &rhs) throw();
};

}

std::istream& operator>>(std::istream& stream, iri::IRIRef &ref);
std::ostream& operator<<(std::ostream& stream, const iri::IRIRef &ref);

#include "iri/IRIRef-inl.h"

#endif /* __IRI__IRIREF_H__ */
