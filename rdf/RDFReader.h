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

#ifndef __RDF__RDFREADER_H__
#define __RDF__RDFREADER_H__

#include "rdf/RDFTriple.h"

namespace rdf {

class RDFReader {
public:
  virtual ~RDFReader() {}
  virtual bool read(RDFTriple &triple) = 0;
  virtual void close() = 0;
};

}

#endif /* __RDF__RDFREADER_H__ */
