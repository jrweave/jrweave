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

#include "ex/BaseException.h"

#include <iostream>
#include "test/unit.h"

using namespace std;
using namespace ex;

void throws() THROWS(BaseException<int>) {
  THROW(BaseException<int>, 5, "five");
} TRACE(BaseException<int>, "unhandled")

bool testThrows() {
  try {
    throws();
  } catch (BaseException<int> &e) {
    cout << e.what();
    return true;
  }
  return false;
}

bool testWrap() {
  try {
    throws();
  } catch (BaseException<int> &e) {
    try {
      THROW(BaseException<TraceableException*>, &e, "middle man");
    } catch (BaseException<TraceableException*> &e) {
      cout << e.what();
      return true;
    }
  }
  return false;
}

int main (int argc, char **argv) {
  INIT;
  TEST(testThrows);
  TEST(testWrap);
  FINAL;
}
