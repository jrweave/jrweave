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

#include "ucs/utf.h"
#include "ucs/UTF8Iter.h"
#include "ucs/UTF16Iter.h"
#include "ucs/UTF32Iter.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "ptr/APtr.h"
#include "test/unit.h"
#include "sys/ints.h"
#include "sys/endian.h"

using namespace ex;
using namespace ptr;
using namespace std;
using namespace sys;

template<typename T>
void printseq(DPtr<T> *p) {
  cerr << endl;
  cerr << "ptr: " << p->ptr() << endl;
  cerr << "size: " << p->size() << endl;
  size_t i;
  for (i = 0; i < p->size(); i++) {
    cerr << hex << (uint32_t)((*p)[i]) << " ";
  }
  cerr << endl;
}

DPtr<uint32_t> *parse(const string &str) {
  vector<uint32_t> vec;
  stringstream ss (stringstream::in | stringstream::out);
  ss << str;
  uint32_t c;
  while (ss.good()) {
    ss >> hex >> c;
    vec.push_back(c);
  }
  APtr<uint32_t> *p;
  NEW(p, APtr<uint32_t>, vec.size());
  copy(vec.begin(), vec.end(), p->dptr());
  return p;
}

bool testRoundTripUTF8(DPtr<uint32_t> *codepoints) {
  DPtr<uint8_t> *enc = ucs::utf8enc(codepoints);
  PROG(enc->sizeKnown());
  DPtr<uint8_t> *enc2;
  NEW(enc2, APtr<uint8_t>, enc->size());
  size_t len = ucs::utf8enc(codepoints->dptr(),
      codepoints->dptr() + codepoints->size(), enc2->dptr());
  PROG(len == enc->size());
  PROG(memcmp(enc->dptr(), enc2->dptr(), enc->size()*sizeof(uint8_t)) == 0);
  enc2->drop();
  ucs::UTF8Iter *begin = ucs::UTF8Iter::begin(enc);
  ucs::UTF8Iter *end = ucs::UTF8Iter::end(enc);
  PROG(begin->more() || codepoints->size() <= 0);
  PROG(!end->more());
  PROG(*begin == *end || codepoints->size() > 0);
  PROG(equal(*begin, *end, codepoints->dptr()));
  begin->start();
  begin->validate(true);
  PROG(true);
  PROG(equal(codepoints->dptr(), codepoints->dptr() + codepoints->size(),
             *begin));
  DELETE(begin);
  DELETE(end);
  DPtr<uint32_t> *found = ucs::utf8dec(enc);
  PROG(found->sizeKnown());
  PROG(codepoints->size() == found->size());
  PROG(memcmp(codepoints->ptr(), found->ptr(),
      codepoints->size() * sizeof(uint32_t)) == 0);
  enc->drop();
  found->drop();
  PASS;
}

bool testRoundTripUTF16(DPtr<uint32_t> *codepoints, enum ucs::BOM bom) {
  DPtr<uint16_t> *enc = ucs::utf16enc(codepoints, bom);
  PROG(enc->sizeKnown());
  if (enc->size() > 0) {
    PROG(bom != ucs::NONE || ((*enc)[0] != UINT16_C(0xFEFF) && (*enc)[0] != UINT16_C(0xFFFE)));
    PROG(bom != ucs::LITTLE || (is_little_endian() && (*enc)[0] == UINT16_C(0xFEFF)) || (is_big_endian() && (*enc)[0] == UINT16_C(0xFFFE)));
    PROG(bom != ucs::BIG || (is_little_endian() && (*enc)[0] == UINT16_C(0xFFFE)) || (is_big_endian() && (*enc)[0] == UINT16_C(0xFEFF)));
  }
  DPtr<uint16_t> *enc2;
  NEW(enc2, APtr<uint16_t>, enc->size());
  size_t len = ucs::utf16enc(codepoints->dptr(),
      codepoints->dptr() + codepoints->size(), bom, enc2->dptr());
  PROG(len == enc->size());
  PROG(memcmp(enc->dptr(), enc2->dptr(), enc->size()*sizeof(uint16_t)) == 0);
  enc2->drop();
  ucs::UTF16Iter *begin = ucs::UTF16Iter::begin(enc);
  ucs::UTF16Iter *end = ucs::UTF16Iter::end(enc);
  PROG(begin->more() || codepoints->size() <= 0);
  PROG(!end->more());
  PROG(*begin == *end || codepoints->size() > 0);
  PROG(equal(*begin, *end, codepoints->dptr()));
  begin->start();
  begin->validate(true);
  PROG(equal(codepoints->dptr(), codepoints->dptr() + codepoints->size(),
             *begin));
  DELETE(begin);
  DELETE(end);
  DPtr<uint32_t> *found = ucs::utf16dec(enc);
  PROG(found->sizeKnown());
  PROG(codepoints->size() == found->size());
  PROG(memcmp(codepoints->ptr(), found->ptr(),
      codepoints->size() * sizeof(uint32_t)) == 0);
  enc->drop();
  found->drop();
  PASS;
}

bool testRoundTripUTF32(DPtr<uint32_t> *codepoints, enum ucs::BOM bom) {
  DPtr<uint32_t> *enc = ucs::utf32enc(codepoints, bom);
  PROG(enc->sizeKnown());
  if (enc->size() > 0) {
    PROG(bom != ucs::NONE || ((*enc)[0] != UINT32_C(0x0000FEFF) && (*enc)[0] != UINT32_C(0xFFFE0000)));
    PROG(bom != ucs::LITTLE || (is_little_endian() && (*enc)[0] == UINT32_C(0x0000FEFF)) || (is_big_endian() && (*enc)[0] == UINT32_C(0xFFFE0000)));
    PROG(bom != ucs::BIG || (is_little_endian() && (*enc)[0] == UINT32_C(0xFFFE0000)) || (is_big_endian() && (*enc)[0] == UINT32_C(0x0000FEFF)));
  }
  DPtr<uint32_t> *enc2;
  NEW(enc2, APtr<uint32_t>, enc->size());
  size_t len = ucs::utf32enc(codepoints->dptr(),
      codepoints->dptr() + codepoints->size(), bom, enc2->dptr());
  PROG(len == enc->size());
  PROG(memcmp(enc->dptr(), enc2->dptr(), enc->size()*sizeof(uint32_t)) == 0);
  enc2->drop();
  ucs::UTF32Iter *begin = ucs::UTF32Iter::begin(enc);
  ucs::UTF32Iter *end = ucs::UTF32Iter::end(enc);
  PROG(begin->more() || codepoints->size() <= 0);
  PROG(!end->more());
  PROG(*begin == *end || codepoints->size() > 0);
  PROG(equal(*begin, *end, codepoints->dptr()));
  begin->start();
  begin->validate(true);
  PROG(equal(codepoints->dptr(), codepoints->dptr() + codepoints->size(),
             *begin));
  DELETE(begin);
  DELETE(end);
  DPtr<uint32_t> *found = ucs::utf32dec(enc);
  PROG(found->sizeKnown());
  PROG(codepoints->size() == found->size());
  PROG(memcmp(codepoints->ptr(), found->ptr(),
      codepoints->size() * sizeof(uint32_t)) == 0);
  enc->drop();
  found->drop();
  PASS;
}

bool testRoundTrips(DPtr<uint32_t> *codepoints) {
  PROG(testRoundTripUTF8(codepoints));
  PROG(testRoundTripUTF16(codepoints, ucs::ANY));
  PROG(testRoundTripUTF16(codepoints, ucs::NONE));
  PROG(testRoundTripUTF16(codepoints, ucs::BIG));
  PROG(testRoundTripUTF16(codepoints, ucs::LITTLE));
  PROG(testRoundTripUTF32(codepoints, ucs::ANY));
  PROG(testRoundTripUTF32(codepoints, ucs::NONE));
  PROG(testRoundTripUTF32(codepoints, ucs::BIG));
  PROG(testRoundTripUTF32(codepoints, ucs::LITTLE));
  PASS;
}

bool testRoundTripsIfDiff(string str1, string str2, string str3) {
  if (str1 == str3 || str2 == str3) {
    PASS;
  }
  DPtr<uint32_t> *p = parse(str3);
  PROG(testRoundTrips(p));
  p->drop();
  PASS;
}

int main(int argc, char **argv) {
  INIT;

  ifstream ifs ("../scripts/NormalizationTest.txt", ifstream::in);
  while (ifs.good()) {

    string line;
    getline(ifs, line);

    if (line.size() <= 0) {
      continue;
    }

    if (('A' > line[0] || line[0] > 'Z') &&
        ('0' > line[0] || line[0] > '9')) {
      continue;
    }

    stringstream ss (stringstream::in | stringstream::out);
    ss << line;
    string str, nfc, nfd, nfkc, nfkd;

    getline(ss, str, ';');
    getline(ss, nfc, ';');
    getline(ss, nfd, ';');
    getline(ss, nfkc, ';');
    getline(ss, nfkd, ';');

    DPtr<uint32_t> *codepoints = parse(str);
    TEST(testRoundTrips, codepoints);
    codepoints->drop();

    TEST(testRoundTripsIfDiff, str, str, nfd);
    TEST(testRoundTripsIfDiff, str, nfd, nfc);
    TEST(testRoundTripsIfDiff, str, nfd, nfkd);
    TEST(testRoundTripsIfDiff, str, nfkd, nfkc);
  }  

  ifs.close();

  DPtr<uint32_t> *codepoints;

  //codepoints = parse(string("26480 35199"));
  codepoints = parse(string("6770 897F"));
  TEST(testRoundTrips, codepoints);
  codepoints->drop();

  NEW(codepoints, APtr<uint32_t>, (size_t)0);
  TEST(testRoundTrips, codepoints);
  codepoints->drop();

  FINAL;
}
