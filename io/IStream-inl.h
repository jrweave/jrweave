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

#include "io/IStream.h"

#include <algorithm>
#include "ptr/MPtr.h"
#include "sys/char.h"

namespace io {

template<typename istream_t>
IStream<istream_t>::IStream(const size_t bufsize) throw(BadAllocException)
    : offset(0), length(0), marked(false) {
  try {
    NEW(this->buffer, MPtr<uint8_t>, bufsize);
  } RETHROW_BAD_ALLOC
  this->mark_support = (this->stream.tellg() != (streampos) -1);
}

template<typename istream_t>
IStream<istream_t>::IStream(istream_t &stream) throw(BadAllocException)
    : stream(stream), offset(0), length(0), marked(false) {
  try {
    NEW(this->buffer, MPtr<uint8_t>, 4096);
  } RETHROW_BAD_ALLOC
  this->mark_support = (this->stream.tellg() != (streampos) -1);
}

template<typename istream_t>
IStream<istream_t>::IStream(istream_t &stream, const size_t bufsize)
    throw(BadAllocException, BaseException<size_t>)
    : stream(stream), offset(0), length(0), marked(false) {
  if (bufsize <= 0) {
    THROW(BaseException<size_t>, 0, "Specified buffer size must be positive.");
  }
  try {
    NEW(this->buffer, MPtr<uint8_t>, bufsize);
  } RETHROW_BAD_ALLOC
  this->mark_support = (this->stream.tellg() != (streampos) -1);
}

template<typename istream_t>
IStream<istream_t>::~IStream() throw(IOException) {
  this->buffer->drop();
}

template<typename istream_t>
void IStream<istream_t>::close() throw(IOException) {
  this->stream.close();
}

template<typename istream_t>
int64_t IStream<istream_t>::available() throw(IOException) {
  if (this->offset < this->length) {
    return this->length - this->offset;
  }
  return min(
    (size_t) (this->stream.rdbuf()->in_avail()
              * sizeof(char) / sizeof(uint8_t)),
    this->buffer->size());
}

template<typename istream_t>
bool IStream<istream_t>::mark(const int64_t read_limit) throw(IOException) {
  this->marked = true;
  this->marker = this->stream.tellg();
  return this->marker != (streampos) -1;
}

template<typename istream_t>
bool IStream<istream_t>::markSupported() const throw() {
  return this->mark_support;
}

template<typename istream_t>
DPtr<uint8_t> *IStream<istream_t>::read(const int64_t amount)
    throw(IOException, BadAllocException) {
  if (this->stream.eof()) {
    return NULL;
  }
  if (!this->stream.good()) {
    THROW(IOException, "Unknown stream error.");
  }
  int64_t avail = max(INT64_C(1), this->available());
  int64_t actual = min(avail, amount);
  if (this->offset >= this->length) {
    if (!this->buffer->alone()) {
      this->buffer = this->buffer->stand();
    }
    uint8_t *p = this->buffer->dptr();
    uint8_t *end = p + actual;
    for (; p != end; ++p) {
      char c = (char) this->stream.get();
      if (this->stream.eof()) {
        if (p == this->buffer->dptr()) {
          return NULL;
        }
        break;
      }
      if (this->stream.fail()) {
        THROW(IOException, "Failed input operation.");
      }
      if (this->stream.bad()) {
        THROW(IOException, "Bad operation on buffer underlying stream.");
      }
      *p = to_ascii(c);
    }
    DPtr<uint8_t> *s = this->buffer->sub(0, p - this->buffer->dptr());
    return s;
  }
  DPtr<uint8_t> *s = this->buffer->sub(this->offset, actual);
  this->offset += actual;
  return s;
}

template<typename istream_t>
void IStream<istream_t>::reset() throw(IOException) {
  if (!this->marked) {
    THROW(IOException, "A mark has not been set.");
  }
  this->stream.seekg(this->marker);
  if (this->stream.fail()) {
    THROW(IOException, "Underlying stream could not reset to mark.");
  }
  if (this->stream.bad()) {
    THROW(IOException, "Unknown stream error.");
  }
}

template<typename istream_t>
int64_t IStream<istream_t>::skip(const int64_t n) throw(IOException) {
  streampos at = this->stream.tellg();
  this->stream.ignore(n);
  if (this->stream.bad()) {
    THROW(IOException, "Unknown stream error.");
  }
  return this->stream.tellg() - at;
}

}
