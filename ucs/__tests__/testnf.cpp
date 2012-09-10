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

#include "ucs/nf.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "ex/TraceableException.h"
#include "ptr/APtr.h"
#include "ptr/DPtr.h"
#include "test/unit.h"
#include "sys/ints.h"

using namespace ex;
using namespace ptr;
using namespace std;

void printerr(const DPtr<uint32_t> *expected, const DPtr<uint32_t> *found) {
#if 0
  size_t i;
  cerr << "\nExpected: ";
  for (i = 0; i < expected->size(); i++) {
    cerr << hex << (*expected)[i] << " ";
  }
  cerr << endl << "Found: ";
  for (i = 0; i < found->size(); i++) {
    cerr << hex << (*found)[i] << " ";
  }
  cerr << endl;
#endif
}

bool testInvalid(DPtr<uint32_t> *codepoints) {
  #if defined(UCS_PLAY_DUMB) || defined(UCS_TRUST_CODEPOINTS)
  bool flip = true;
  #else
  bool flip = false;
  #endif
  size_t i;
  PROG(codepoints->sizeKnown());
  bool is_valid = true;
  for (i = 0; i < codepoints->size(); i++) {
    is_valid &= ucs::nfvalid((*codepoints)[i]);
  }
  PROG((flip && is_valid) || (!flip && !is_valid));
  try {
    DPtr<uint32_t> *p = ucs::nfd(codepoints);
    p->drop();
    PROG(flip);
  } catch (ucs::InvalidCodepointException &e) {
    PROG(!flip);
  }
  #ifndef UCS_NO_K
  try {
    DPtr<uint32_t> *p = ucs::nfkd(codepoints);
    p->drop();
    PROG(flip);
  } catch (ucs::InvalidCodepointException &e) {
    PROG(!flip);
  }
  #endif
  #ifndef UCS_NO_C
  try {
    DPtr<uint32_t> *p = ucs::nfc(codepoints);
    p->drop();
    PROG(flip);
  } catch (ucs::InvalidCodepointException &e) {
    PROG(!flip);
  }
  #ifndef UCS_NO_K
  try {
    DPtr<uint32_t> *p = ucs::nfkc(codepoints);
    p->drop();
    PROG(flip);
  } catch (ucs::InvalidCodepointException &e) {
    PROG(!flip);
  }
  #endif
  #endif
  PASS;
}

bool testQuickCheck(const DPtr<uint32_t> *input, bool not_no, bool do_c, bool do_k) {
  uint8_t qc;
  if (do_c) {
    #ifdef UCS_NO_C
    THROW(TraceableException, "This should never happen.");
    #else
    if (do_k) {
      #ifdef UCS_NO_K
      THROW(TraceableException, "This should never happen.");
      #else
      qc = ucs::nfkc_qc(input, NULL);
      #endif
    } else {
      qc = ucs::nfc_qc(input, NULL);
    }
    #endif
  } else {
    if (do_k) {
      #ifdef UCS_NO_K
      THROW(TraceableException, "This should never happen.");
      #else
      qc = ucs::nfkd_qc(input, NULL);
      #endif
    } else {
      qc = ucs::nfd_qc(input, NULL);
    }
  }
  cerr << "QC: " << (int)qc << endl;
  PROG((qc == (not_no ? UCS_QC_YES : UCS_QC_NO)) || qc == UCS_QC_MAYBE);
  PASS;
}

bool test(const DPtr<uint32_t> *expected, const DPtr<uint32_t> *found) {
  size_t i;
  PROG(expected->sizeKnown());
  PROG(found->sizeKnown());
  printerr(expected, found);
  PROG(found->size() == expected->size());
  for (i = 0; i < found->size(); i++) {
    PROG((*expected)[i] == (*found)[i]);
  }
  PASS;
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
    
    DPtr<uint32_t> *input;
    DPtr<uint32_t> *expected;
    DPtr<uint32_t> *found;

    input = parse(str);

    cerr << "INPUT " << str << endl;
    cerr << "NFD " << nfd << endl;
#ifdef UCS_PLAY_DUMB
    nfd = str;
#endif
    TEST(testQuickCheck, input, nfd == str, false, false);
    expected = parse(nfd);
    found = ucs::nfd(input);
    TEST(test, expected, found);
    found->drop();
    found = ucs::nfd_opt(input);
    TEST(test, expected, found);
    found->drop();
    expected->drop();

#ifndef UCS_NO_K
    cerr << "INPUT " << str << endl;
    cerr << "NFKD " << nfkd << endl;
#ifdef UCS_PLAY_DUMB
    nfkd = str;
#endif
    TEST(testQuickCheck, input, nfkd == str, false, true);
    expected = parse(nfkd);
    found = ucs::nfkd(input);
    TEST(test, expected, found);
    found->drop();
    found = ucs::nfkd_opt(input);
    TEST(test, expected, found);
    found->drop();
    expected->drop();
#endif

#ifndef UCS_NO_C
    cerr << "INPUT " << str << endl;
    cerr << "NFC " << nfc << endl;
#ifdef UCS_PLAY_DUMB
    nfc = str;
#endif
    TEST(testQuickCheck, input, nfc == str, true, false);
    expected = parse(nfc);
    found = ucs::nfc(input);
    TEST(test, expected, found);
    found->drop();
    found = ucs::nfc_opt(input);
    TEST(test, expected, found);
    found->drop();
    expected->drop();

#ifndef UCS_NO_K
    cerr << "INPUT " << str << endl;
    cerr << "NFKC " << nfkc << endl;
#ifdef UCS_PLAY_DUMB
    nfkc = str;
#endif
    TEST(testQuickCheck, input, nfkc == str, true, true);
    expected = parse(nfkc);
    found = ucs::nfkc(input);
    TEST(test, expected, found);
    found->drop();
    found = ucs::nfkc_opt(input);
    TEST(test, expected, found);
    found->drop();
    expected->drop();
#endif
#endif

    input->drop();
  }

  ifs.close();
/*
  DPtr<uint32_t> *p;
  NEW(p, APtr<uint32_t>, 1);
  (*p)[0] = UINT32_C(0x0378);
  TEST(testInvalid, p);
  p->drop();
*/
  FINAL;
}
