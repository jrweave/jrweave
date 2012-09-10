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

#ifndef __RDF__RDFENCODER_H__
#define __RDF__RDFENCODER_H__

#include "ex/TraceableException.h"
#include "rdf/RDFTerm.h"
#include "sys/ints.h"

namespace rdf {

using namespace ex;
using namespace std;

template<size_t N>
class RDFID {
private:
  uint8_t bytes[N];
public:
  RDFID() {}
  RDFID(const int init) {
    memcpy(this->bytes + N - sizeof(int), &init, sizeof(int));
    memset(this->bytes, 0, N - sizeof(int));
  }
  RDFID(const RDFID<N> &copy) {
    memcpy(this->bytes, copy.bytes, N);
  }
  ~RDFID() {}
  static RDFID<N> min() {
    return RDFID<N>::zero();
  }
  static RDFID<N> zero() {
    RDFID<N> id;
    memset(id.bytes, 0, N);
    return id;
  }
  static RDFID<N> max() {
    RDFID<N> id;
    memset(id.bytes, UINT8_C(0xFF), N);
    return id;
  }
  static size_t size() { return N; }
  bool operator[](const size_t i) const {
    return ((this->bytes[N - (i >> 3) - 1] >> (i & 0x7)) & 0x1) != 0;
  }
  bool operator()(const size_t i, const bool v) {
    size_t byteoff = N - (i >> 3) - 1;
    size_t bitoff = i & 0x7;
    bool oldv = (this->bytes[byteoff] >> bitoff) != 0;
    if (oldv ^ v) {
      this->bytes[byteoff] ^= (1 << bitoff);
    }
    return oldv;
  }
  const uint8_t *ptr() const { return &bytes[0]; }
  uint8_t *ptr() { return &bytes[0]; }
  RDFID<N> &operator++() {
    size_t i = N;
    for (; i > 0; --i) {
      if (++this->bytes[i-1] != UINT8_C(0)) {
        break;
      }
    }
    return *this;
  }
  void operator++(int) {
    RDFID<N> prev (*this);
    size_t i = N;
    for (; i > 0; --i) {
      if (++this->bytes[i-1] != UINT8_C(0)) {
        break;
      }
    }
  }
  RDFID<N> &operator^=(const RDFID<N> &id) {
    size_t i = 0;
    for (; i < N; ++i) {
      this->bytes[i] ^= id.bytes[i];
    }
    return *this;
  }
  RDFID<N> operator^(const RDFID<N> &id) const {
    return RDFID<N>(*this) ^= id;
  }
  RDFID<N> &operator&=(const RDFID<N> &id) {
    size_t i = 0;
    for (; i < N; ++i) {
      this->bytes[i] &= id.bytes[i];
    }
    return *this;
  }
  RDFID<N> operator&(const RDFID<N> &id) const {
    return RDFID<N>(*this) &= id;
  }
  RDFID<N> &operator|=(const RDFID<N> &id) {
    size_t i = 0;
    for (; i < N; ++i) {
      this->bytes[i] |= id.bytes[i];
    }
    return *this;
  }
  RDFID<N> operator|(const RDFID<N> &id) const {
    return RDFID<N>(*this) |= id;
  }
  RDFID<N> &operator<<=(const size_t k) {
    size_t i = 0;
    size_t byteoff = k >> 3;
    if (byteoff < N) {
      size_t bitoff = k & 0x7;
      for (; i < N - byteoff - 1; ++i) {
        this->bytes[i] = (this->bytes[i+byteoff] << bitoff)
          | (this->bytes[i+byteoff+1] >> (8 - bitoff));
      }
      this->bytes[i] = this->bytes[i+byteoff] << bitoff;
      ++i;
    }
    memset(this->bytes + i, 0, N - i);
    return *this;
  }
  RDFID<N> operator<<(const size_t k) const {
    return RDFID<N>(*this) <<= k;
  }
  RDFID<N> &operator>>=(const size_t k) {
    size_t i = N;
    size_t byteoff = k >> 3;
    if (byteoff < N) {
      size_t bitoff = k & 0x7;
      for (--i; i > byteoff; --i) {
        this->bytes[i] = (this->bytes[i-byteoff] >> bitoff)
          | (this->bytes[i-byteoff-1] << (8 - bitoff));
      }
      this->bytes[i] = this->bytes[i-byteoff] >> bitoff;
    }
    memset(this->bytes, 0, i);
    return *this;
  }
  RDFID<N> operator>>(const size_t k) const {
    return RDFID<N>(*this) >>= k;
  }
  bool operator<(const RDFID<N> &id) const {
    return memcmp(this->bytes, id.bytes, N) < 0;
  }
  bool operator<=(const RDFID<N> &id) const {
    return memcmp(this->bytes, id.bytes, N) <= 0;
  }
  bool operator==(const RDFID<N> &id) const {
    return memcmp(this->bytes, id.bytes, N) == 0;
  }
  bool operator>=(const RDFID<N> &id) const {
    return memcmp(this->bytes, id.bytes, N) >= 0;
  }
  bool operator>(const RDFID<N> &id) const {
    return memcmp(this->bytes, id.bytes, N) > 0;
  }
  bool operator!=(const RDFID<N> &id) const {
    return memcmp(this->bytes, id.bytes, N) != 0;
  }
  RDFID<N> &operator=(const RDFID<N> &id) {
    memcpy(this->bytes, id.bytes, N);
    return *this;
  }
};

//template<>                                            
//class RDFID<1> {                                      
//private:                                              
//  uint8_t bytes;                                        
//public:                                               
//  RDFID() {}                                          
//  RDFID(const RDFID<1> &copy) : bytes(copy.bytes) {}  
//  ~RDFID() {}                                         
//  static RDFID<1> min() {                             
//    RDFID<1> id; id.bytes = 0; return id;           
//  }                                                   
//  static RDFID<1> zero() {                            
//    RDFID<1> id; id.bytes = 0; return id;             
//  }                                                   
//  static RDFID<1> max() {                             
//    RDFID<1> id; id.bytes = UINT8_MAX; return id;           
//  }                                                   
//  static size_t size() { return 1; }                  
//  bool operator[](const size_t i) const {                   
//    return ((this->bytes >> i) & 1) != 0;             
//  }                                                   
//  bool operator()(const size_t i, const bool v) {     
//    bool oldv = (*this)[i];                           
//    if (oldv ^ v) {                                   
//      this->bytes ^= (1 << i);                        
//    }                                                 
//    return oldv;                                      
//  }                                                   
//  uint8_t *ptr() {                              
//    return (uint8_t*)(&this->bytes);                  
//  }                                                   
//  const uint8_t *ptr() const {                              
//    return (uint8_t*)(&this->bytes);                  
//  }                                                   
//  RDFID<1> &operator++() { ++this->bytes; return *this;}    
//  void operator++(int) { ++this->bytes; }             
//  RDFID<1> &operator^=(const RDFID<1> &id) {          
//    this->bytes ^= id.bytes;                          
//    return *this;                                     
//  }                                                   
//  RDFID<1> operator^(const RDFID<1> &id) const {           
//    RDFID<1> i;
//    i.bytes = this->bytes ^ id.bytes;
//    return i; 
//  }                                                   
//  RDFID<1> &operator&=(const RDFID<1> &id) {          
//    this->bytes &= id.bytes;                          
//    return *this;                                     
//  }                                                   
//  RDFID<1> operator&(const RDFID<1> &id) const {            
//    RDFID<1> i;
//    i.bytes = this->bytes & id.bytes;
//    return i; 
//  }                                                   
//  RDFID<1> &operator|=(const RDFID<1> &id) {          
//    this->bytes |= id.bytes;                          
//    return *this;                                     
//  }                                                   
//  RDFID<1> operator|(const RDFID<1> &id) const {            
//    RDFID<1> i;
//    i.bytes = this->bytes | id.bytes;
//    return i; 
//  }                                                   
//  RDFID<1> operator<<=(const size_t k) {              
//    this->bytes << k;                                 
//    return *this;                                     
//  }                                                   
//  RDFID<1> operator<<(const size_t k) const {               
//    RDFID<1> id;
//    id.bytes = this->bytes << k;
//    return id;
//  }                                                   
//  RDFID<1> operator>>=(const size_t k) {              
//    this->bytes >> k;                                 
//    return *this;                                     
//  }                                                   
//  RDFID<1> operator>>(const size_t k) const {               
//    RDFID<1> id;
//    id.bytes = this->bytes >> k;
//    return id;
//  }                                                   
//  bool operator<(const RDFID<1> &id) const {                
//    return this->bytes < id.bytes;                    
//  }                                                   
//  bool operator<=(const RDFID<1> &id) const {               
//    return this->bytes <= id.bytes;                   
//  }                                                   
//  bool operator==(const RDFID<1> &id) const {               
//    return this->bytes == id.bytes;                   
//  }                                                   
//  bool operator>=(const RDFID<1> &id) const {               
//    return this->bytes >= id.bytes;                   
//  }                                                   
//  bool operator>(const RDFID<1> &id) const {                
//    return this->bytes > id.bytes;                    
//  }                                                   
//  bool operator!=(const RDFID<1> &id) const {               
//    return this->bytes != id.bytes;                   
//  }                                                   
//  RDFID<1> &operator=(const RDFID<1> &id) {           
//    this->bytes = id.bytes;                           
//    return *this;                                     
//  }                                                   
//};                                                    
//typedef RDFID<1> rdf1_t;

#define SPECIALIZE_RDFID(N, int_t, MIN, MAX)          \
template<>                                            \
class RDFID<N> {                                      \
private:                                              \
  int_t bytes;                                        \
public:                                               \
  RDFID() {}                                          \
  RDFID(const int init) : bytes((int_t)init) {}       \
  RDFID(const RDFID<N> &copy) : bytes(copy.bytes) {}  \
  ~RDFID() {}                                         \
  static RDFID<N> min() {                             \
    RDFID<N> id; id.bytes = MIN; return id;           \
  }                                                   \
  static RDFID<N> zero() {                            \
    RDFID<N> id; id.bytes = 0; return id;             \
  }                                                   \
  static RDFID<N> max() {                             \
    RDFID<N> id; id.bytes = MAX; return id;           \
  }                                                   \
  static size_t size() { return N; }                  \
  bool operator[](const size_t i) const {             \
    return ((this->bytes >> i) & 1) != 0;             \
  }                                                   \
  bool operator()(const size_t i, const bool v) {     \
    bool oldv = (*this)[i];                           \
    if (oldv ^ v) {                                   \
      this->bytes ^= (1 << i);                        \
    }                                                 \
    return oldv;                                      \
  }                                                   \
  const uint8_t *ptr() const {                        \
    return (uint8_t*)(&this->bytes);                  \
  }                                                   \
  uint8_t *ptr() {                                    \
    return (uint8_t*)(&this->bytes);                  \
  }                                                   \
  RDFID<N> &operator++() {                            \
    ++this->bytes;                                    \
    return *this;                                     \
  }                                                   \
  void operator++(int) { ++this->bytes; }             \
  RDFID<N> &operator^=(const RDFID<N> &id) {          \
    this->bytes ^= id.bytes;                          \
    return *this;                                     \
  }                                                   \
  RDFID<N> operator^(const RDFID<N> &id) const {      \
    RDFID<N> i;                                       \
    i.bytes = this->bytes ^ id.bytes;                 \
    return i;                                         \
  }                                                   \
  RDFID<N> &operator&=(const RDFID<N> &id) {          \
    this->bytes &= id.bytes;                          \
    return *this;                                     \
  }                                                   \
  RDFID<N> operator&(const RDFID<N> &id) const {      \
    RDFID<N> i;                                       \
    i.bytes = this->bytes & id.bytes;                 \
    return i;                                         \
  }                                                   \
  RDFID<N> &operator|=(const RDFID<N> &id) {          \
    this->bytes |= id.bytes;                          \
    return *this;                                     \
  }                                                   \
  RDFID<N> operator|(const RDFID<N> &id) const {      \
    RDFID<N> i;                                       \
    i.bytes = this->bytes | id.bytes;                 \
    return i;                                         \
  }                                                   \
  RDFID<N> operator<<=(const size_t k) {              \
    this->bytes <<= k;                                \
    return *this;                                     \
  }                                                   \
  RDFID<N> operator<<(const size_t k) const {         \
    RDFID<N> id;                                      \
    id.bytes = this->bytes << k;                      \
    return id;                                        \
  }                                                   \
  RDFID<N> operator>>=(const size_t k) {              \
    this->bytes >>= k;                                \
    return *this;                                     \
  }                                                   \
  RDFID<N> operator>>(const size_t k) const {         \
    RDFID<N> id;                                      \
    id.bytes = this->bytes >> k;                      \
    return id;                                        \
  }                                                   \
  bool operator<(const RDFID<N> &id) const {          \
    return this->bytes < id.bytes;                    \
  }                                                   \
  bool operator<=(const RDFID<N> &id) const {         \
    return this->bytes <= id.bytes;                   \
  }                                                   \
  bool operator==(const RDFID<N> &id) const {         \
    return this->bytes == id.bytes;                   \
  }                                                   \
  bool operator>=(const RDFID<N> &id) const {         \
    return this->bytes >= id.bytes;                   \
  }                                                   \
  bool operator>(const RDFID<N> &id) const {          \
    return this->bytes > id.bytes;                    \
  }                                                   \
  bool operator!=(const RDFID<N> &id) const {         \
    return this->bytes != id.bytes;                   \
  }                                                   \
  RDFID<N> &operator=(const RDFID<N> &id) {           \
    this->bytes = id.bytes;                           \
    return *this;                                     \
  }                                                   \
};                                                    \
typedef RDFID<N> rdf##N##_t;

// TODO #if BIG_ENDIAN allow these
//SPECIALIZE_RDFID(1,  uint8_t,  UINT8_C(0),  UINT8_MAX)
//SPECIALIZE_RDFID(2, uint16_t, UINT16_C(0), UINT16_MAX)
//SPECIALIZE_RDFID(4, uint32_t, UINT32_C(0), UINT32_MAX)
//SPECIALIZE_RDFID(8, uint64_t, UINT64_C(0), UINT64_MAX)

template<typename ID>
class RDFEncoder {
public:
  bool operator()(const RDFTerm &term, ID &id) { return false; }
  bool operator()(const ID &id, RDFTerm &term) { return false; }
};

}

#endif /* __RDF__RDFENCODER_H__ */
