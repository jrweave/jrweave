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

#include "rdf/NTriplesWriter.h"

#include <ctime>
#include <deque>
#include "io/IOException.h"
#include "ucs/utf.h"

namespace rdf {

using namespace io;
using namespace std;
using namespace ucs;

NTriplesWriter::NTriplesWriter(OutputStream *os) throw(BaseException<void*>)
    : output(os) {
  if (os == NULL) {
    THROW(BaseException<void*>, NULL, "OutputStream *os must not be NULL.");
  }
}

NTriplesWriter::~NTriplesWriter() throw() {
  DELETE(this->output);
}

DPtr<uint8_t> *NTriplesWriter::escapeUnicode(DPtr<uint8_t> *utf8str)
    THROWS(BadAllocException, InvalidEncodingException) {
  const uint8_t *begin = utf8str->dptr();
  const uint8_t *end = begin + utf8str->size();
  const uint8_t *mark;
  const uint8_t *next;
  uint32_t cp;
  for (mark = begin; mark != end; mark = next) {
    cp = utf8char(mark, &next);
    if (cp > UINT32_C(0x7F)) {
      break; // found non-ASCII character
    }
  }
  if (mark == end) {
    utf8str->hold();
    return utf8str;
  }
  deque<uint8_t> deq;
  do {
    deq.insert(deq.end(), begin, mark);
    bool is_short = cp <= UINT32_C(0xFFFF);
    deq.push_back(to_ascii('\\'));
    deq.push_back(to_ascii((is_short ? 'u' : 'U')));
    size_t i = (is_short ? 4 : 0);
    for (; i < 8; ++i) {
      uint32_t nibble = (cp << (i << 2));
      nibble >>= 28;
      if (nibble <= UINT8_C(9)) {
        deq.push_back((uint8_t)(to_ascii('0') + nibble));
      } else {
        deq.push_back((uint8_t)(to_ascii('A') + nibble - 10));
      }
    }
    begin = next;
    for (mark = begin; mark != end; mark = next) {
      cp = utf8char(mark, &next);
      if (cp > UINT32_C(0x7F)) {
        break; // found non-ASCII character
      }
    }
  } while (mark != end);
  deq.insert(deq.end(), begin, mark);
  DPtr<uint8_t> *esc;
  try {
    NEW(esc, MPtr<uint8_t>, deq.size());
  } RETHROW_BAD_ALLOC
  copy(deq.begin(), deq.end(), esc->dptr());
  return esc;
}
TRACE(InvalidEncodingException, "When Unicode-escaping line.")

RDFTerm NTriplesWriter::sanitize(const RDFTerm &bnode)
    THROWS(BadAllocException) {
  static unsigned int count = 0;
  if (bnode.getType() != BNODE) {
    return bnode;
  }
  DPtr<uint8_t> *label = bnode.getLabel();
  if (label == NULL || label->size() == 0) {
    if (label != NULL) {
      label->drop();
    }
    deque<uint8_t> deq;
    deq.push_back(to_ascii('Q'));
    time_t ts = time(NULL);
    size_t max = sizeof(unsigned int) << 1;
    size_t i;
    for (i = 0; i < max; ++i) {
      unsigned int nibble = (count << (i << 2));
      nibble >>= (sizeof(unsigned int) - 4);
      if (nibble <= UINT8_C(9)) {
        deq.push_back((uint8_t)(to_ascii('0') + nibble));
      } else {
        deq.push_back((uint8_t)(to_ascii('A') + nibble - 10));
      }
    }
    max = sizeof(time_t) << 1;
    for (i = 0; i < max; ++i) {
      time_t nibble = (ts << (i << 2));
      nibble >>= (sizeof(time_t) - 4);
      if (nibble <= UINT8_C(9)) {
        deq.push_back((uint8_t)(to_ascii('0') + nibble));
      } else {
        deq.push_back((uint8_t)(to_ascii('A') + nibble - 10));
      }
    }
    try {
      NEW(label, MPtr<uint8_t>, deq.size());
    } RETHROW_BAD_ALLOC
    copy(deq.begin(), deq.end(), label->dptr());
    RDFTerm ret(label);
    label->drop();
    return ret;
  }
  const uint8_t *begin = label->dptr();
  const uint8_t *end = begin + label->size();
  const uint8_t *mark = begin;
  for (; mark != end; ++mark) {
    if (!is_alnum(*mark)) {
      break; // needs encoding
    }
  }
  if (mark == end) {
    if (*begin != to_ascii('Q') && *begin != to_ascii('X') &&
        *begin != to_ascii('Z') && !is_digit(*begin)) {
      label->drop();
      return bnode;
    }
    DPtr<uint8_t> *newlabel;
    try {
      NEW(newlabel, MPtr<uint8_t>, label->size() + 1);
    } RETHROW_BAD_ALLOC
    (*newlabel)[0] = to_ascii('X');
    memcpy(newlabel->dptr() + 1, label->dptr(), label->size());
    label->drop();
    RDFTerm ret(newlabel);
    newlabel->drop();
    return ret;
  }
  deque<uint8_t> deq;
  deq.push_back(to_ascii('Z'));
  deq.insert(deq.end(), begin, mark);
  do {
    if (is_alnum(*mark) && *mark != to_ascii('Z')) {
      deq.push_back(*mark);
    } else {
      size_t i;
      deq.push_back(to_ascii('Z'));
      for (i = 0; i < 2; ++i) {
        uint8_t nibble = (uint8_t)(*mark << (i << 2));
        nibble >>= 4;
        if (nibble <= UINT8_C(9)) {
          deq.push_back(to_ascii('0') + nibble);
        } else {
          deq.push_back(to_ascii('A') + nibble - UINT8_C(10));
        }
      }
    }
  } while (++mark != end);
  label->drop();
  try {
    NEW(label, MPtr<uint8_t>, deq.size());
  } RETHROW_BAD_ALLOC
  copy(deq.begin(), deq.end(), label->dptr());
  RDFTerm ret(label);
  label->drop();
  return ret;
}
TRACE(InvalidEncodingException, "Couldn't asciify string.")

void NTriplesWriter::write(const RDFTriple &triple) {
  DPtr<uint8_t> *line;
  if (triple.getSubjType() == BNODE || triple.getObjType() == BNODE) {
    RDFTerm subj = triple.getSubj();
    RDFTerm pred = triple.getPred();
    RDFTerm obj = triple.getObj();
    if (subj.getType() == BNODE) {
      try {
        subj = NTriplesWriter::sanitize(subj);
      } JUST_RETHROW(BadAllocException, "(rethrow)")
    }
    if (obj.getType() == BNODE) {
      try {
        obj = NTriplesWriter::sanitize(obj);
      } JUST_RETHROW(BadAllocException, "(rethrow)")
    }
    line = RDFTriple(subj, pred, obj).toUTF8String(true);
  } else {
    try {
      line = triple.toUTF8String(true);
    } JUST_RETHROW(BadAllocException, "Couldn't stringify RDFTriple.")
  }
  DPtr<uint8_t> *escaped;
  try {
    escaped = NTriplesWriter::escapeUnicode(line);
  } catch (BadAllocException &e) {
    line->drop();
    RETHROW(e, "Couldn't escape Unicode characters.");
  }
  line->drop();
  try {
    this->output->write(escaped);
  } catch (IOException &e) {
    escaped->drop();
    RETHROW(e, "Couldn't write triple.");
  }
  escaped->drop();
}

void NTriplesWriter::close() {
  try {
    this->output->close();
  } JUST_RETHROW(IOException, "Couldn't close underlying OutputStream.")
}

}
