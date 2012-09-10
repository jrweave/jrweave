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

#include "rdf/NTriplesReader.h"

namespace rdf {

NTriplesReader::NTriplesReader(InputStream *is) throw()
    : input(is), buffer(NULL), offset(0) {
  if (this->input != NULL) {
    this->buffer = this->input->read();
  }
}

NTriplesReader::~NTriplesReader() throw() {
  if (this->input != NULL) {
    DELETE(this->input);
    if (this->buffer != NULL) {
      this->buffer->drop();
    }
  }
}

bool NTriplesReader::read(RDFTriple &triple) {
  if (this->buffer == NULL) {
    return false;
  }
  if (this->offset >= this->buffer->size()) {
    this->buffer->drop();
    try {
      this->buffer = this->input->read();
    } JUST_RETHROW(IOException, "(rethrow)")
    if (this->buffer == NULL) {
      return false;
    }
    this->offset = 0;
  }
  const uint8_t *begin = this->buffer->dptr() + this->offset;
  const uint8_t *end = this->buffer->dptr() + this->buffer->size();
  const uint8_t *p;
  do {
    for (p = begin; p != end && *p == to_ascii('\n'); ++p) {
      // skip blank lines
    }
    if (p == end) {
      this->buffer->drop();
      try {
        this->buffer = this->input->read();
      } JUST_RETHROW(IOException, "(rethrow)")
      if (this->buffer == NULL) {
        return false;
      }
      this->offset = 0;
      begin = this->buffer->dptr();
      end = begin + this->buffer->size();
    }
  } while (p == end);
  for (; p != end && *p != to_ascii('\n'); ++p) {
    // find end of line/triple
  }
  DPtr<uint8_t> *triplestr = this->buffer->sub(this->offset, p - begin);
  this->offset += triplestr->size() + 1; // skip '\n'
  if (p != end) {
    try {
      triple = RDFTriple::parse(triplestr);
    } catch (BadAllocException &e) {
      triplestr->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidEncodingException &e) {
      triplestr->drop();
      RETHROW(e, "(rethrow)");
    } catch (InvalidCodepointException &e) {
      triplestr->drop();
      RETHROW(e, "(rethrow)");
    } catch (MalformedIRIRefException &e) {
      triplestr->drop();
      RETHROW(e, "(rethrow)");
    } catch (MalformedLangTagException &e) {
      triplestr->drop();
      RETHROW(e, "(rethrow)");
    } catch (BaseException<IRIRef> &e) {
      triplestr->drop();
      RETHROW(e, "(rethrow)");
    } catch (TraceableException &e) {
      triplestr->drop();
      RETHROW(e, "(rethrow)");
    }
    triplestr->drop();
    return true;
  }
  vector<void*> parts;
  if (!triplestr->standable()) {
    THROW(TraceableException, "CANNOT STAND POINTER!");
  }
  triplestr = triplestr->stand();
  size_t len = triplestr->size();
  parts.push_back((void*) triplestr);
  do {
    this->buffer->drop();
    try {
      this->buffer = this->input->read();
    } catch (IOException &e) {
      vector<void*>::iterator it = parts.begin();
      for (; it != parts.end(); ++it) {
        ((DPtr<uint8_t>*)(*it))->drop();
      }
      RETHROW(e, "(rethrow)");
    }
    if (this->buffer == NULL) {
      break;
    }
    begin = this->buffer->dptr();
    end = begin + this->buffer->size();
    p = begin;
    for (; p != end && *p != to_ascii('\n'); ++p) {
      // find end of line/triple
    }
    triplestr = this->buffer->sub(0, p - begin);
    this->offset = triplestr->size() + 1;
    if (!triplestr->standable()) {
      THROW(TraceableException, "CANNOT STAND POINTER!");
    }
    triplestr->stand();
    len += triplestr->size();
    parts.push_back((void*) triplestr);
  } while (p == end);
  if (len <= 0) {
    vector<void*>::iterator it = parts.begin();
    for (; it != parts.end(); ++it) {
      ((DPtr<uint8_t>*)(*it))->drop();
    }
    return false;
  }
  try {
    NEW(triplestr, MPtr<uint8_t>, len);
  } catch (bad_alloc &e) {
    vector<void*>::iterator it = parts.begin();
    for (; it != parts.end(); ++it) {
      ((DPtr<uint8_t>*)(*it))->drop();
    }
    THROW(BadAllocException, sizeof(MPtr<uint8_t>));
  } catch (BadAllocException &e) {
    vector<void*>::iterator it = parts.begin();
    for (; it != parts.end(); ++it) {
      ((DPtr<uint8_t>*)(*it))->drop();
    }
    RETHROW(e, "(rethrow)");
  }
  uint8_t *write_to = triplestr->dptr();
  vector<void*>::iterator it = parts.begin();
  for (; it != parts.end(); ++it) {
    DPtr<uint8_t> *part = (DPtr<uint8_t>*)(*it);
    memcpy(write_to, part->dptr(), part->size() * sizeof(uint8_t));
    write_to += part->size();
    part->drop();
  }
  try {
    triple = RDFTriple::parse(triplestr);
  } catch (BadAllocException &e) {
    triplestr->drop();
    RETHROW(e, "(rethrow)");
  } catch (InvalidEncodingException &e) {
    triplestr->drop();
    RETHROW(e, "(rethrow)");
  } catch (InvalidCodepointException &e) {
    triplestr->drop();
    RETHROW(e, "(rethrow)");
  } catch (MalformedIRIRefException &e) {
    triplestr->drop();
    RETHROW(e, "(rethrow)");
  } catch (MalformedLangTagException &e) {
    triplestr->drop();
    RETHROW(e, "(rethrow)");
  } catch (TraceableException &e) {
    triplestr->drop();
    RETHROW(e, "(rethrow)");
  }
  triplestr->drop();
  return true;
}

void NTriplesReader::close() {
  try {
    this->input->close();
  } JUST_RETHROW(IOException, "(rethrow)")
}

}
