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

#include "test/unit.h"
#include "rdf/NTriplesReader.h"

#include "io/IFStream.h"
#include "ptr/DPtr.h"
#include "rdf/RDFTriple.h"

using namespace io;
using namespace rdf;
using namespace std;

bool test1() {
  InputStream *is;
  NEW(is, IFStream, "foaf.nt");
  size_t count = 0;
  size_t nerrs = 0;
  DPtr<uint8_t> *bytes = is->read();
  while (bytes != NULL) {
    count += bytes->size();
    bytes->drop();
    bytes = is->read();
  }
  is->close();
  DELETE(is);
  PROG(count == 11137);

  NEW(is, IFStream, "foaf.nt");
  NTriplesReader *nt;
  NEW(nt, NTriplesReader, is);
  RDFTriple triple;
  count = 0;
  bool r;
  do {
    try {
      r = nt->read(triple);
      if (r) ++count;
    } catch (TraceableException &e) {
      cerr << e.what() << endl;
      ++nerrs;
    }
  } while(r);
  PROG(nerrs == 0);
  PROG(count == 94);
  nt->close();
  DELETE(nt);
  PASS;
}

int main(int argc, char **argv) {
  INIT;

  TEST(test1);

  FINAL;
}
