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
#include <iostream>
#include <new>
#include <sstream>
#include <vector>
#include "ptr/MPtr.h"

#ifdef UCS_PLAY_DUMB
#warning "No UCS normalization will actually occur since UCS_PLAY_DUMB is defined.  This is probably for performance optimization.  Be sure that this is the desired behavior.\n"
#endif

#ifdef UCS_TRUST_CODEPOINTS
#warning "UCS codepoints will not be validated since UCS_TRUST_CODEPOINTS is defined.  This is probably for performance optimization.  Be sure that this is the desired behavior.\n"
#endif

namespace ucs {

using namespace ex;
using namespace ptr;
using namespace std;

#if !defined(UCS_PLAY_DUMB)
#include "ucs_arrays.cpp"
const uint32_t *nflookupd(const uint32_t codepoint)
    throw(InvalidCodepointException) {
  const uint32_t *ub = upper_bound(UCS_DECOMPOSITION_RANGES,
      UCS_DECOMPOSITION_RANGES + UCS_DECOMPOSITION_RANGES_LEN, codepoint);
  if (ub == UCS_DECOMPOSITION_RANGES) {
    if (!nfvalid(codepoint)) {
      THROW(InvalidCodepointException, codepoint);
    }
    return NULL;
  }
  ub--;
  uint32_t offset = ub - UCS_DECOMPOSITION_RANGES;
  if ((offset & UINT32_C(1)) != UINT32_C(0)) {
    if (!nfvalid(codepoint)) {
      THROW(InvalidCodepointException, codepoint);
    }
    return NULL;
  }
  offset = UCS_DECOMPOSITION_OFFSETS[offset >> 1] + (codepoint - *ub);
  return UCS_DECOMPOSITIONS[offset];
}
#endif

#if !defined(UCS_TRUST_CODEPOINTS) && !defined(UCS_PLAY_DUMB)
bool nfvalid(const uint32_t codepoint) throw() {
  if (codepoint < UCS_CODEPOINT_RANGES[1]) {
    // most common case, includes ASCII
    return true;
  }
  const uint32_t *ub = upper_bound(UCS_CODEPOINT_RANGES,
      UCS_CODEPOINT_RANGES + UCS_CODEPOINT_RANGES_LEN, codepoint);
  if (ub == UCS_CODEPOINT_RANGES) {
    return false;
  }
  ub--;
  uint32_t offset = ub - UCS_CODEPOINT_RANGES;
  return (offset & UINT32_C(1)) == UINT32_C(0);
}
#endif

#if !defined(UCS_TRUST_CODEPOINTS) && !defined(UCS_PLAY_DUMB)
bool nfvalid(DPtr<uint32_t> *codepoints) throw(SizeUnknownException) {
  if (!codepoints->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint32_t *begin = codepoints->dptr();
  const uint32_t *end = begin + codepoints->size();
  for (; begin != end; ++begin) {
    if (!nfvalid(*begin)) {
      return false;
    }
  }
  return true;
}
#endif

#if !defined(UCS_PLAY_DUMB)
DPtr<uint32_t> *nfreturn(DPtr<uint32_t> *p) throw() {
  uint32_t *begin = p->dptr();
  uint32_t *end = begin + p->size();
  for (; begin != end; ++begin) {
    *begin = UCS_UNPACK_CODEPOINT(*begin);
  }
  return p;
}
#endif

#if !defined(UCS_PLAY_DUMB)
template<typename iter_type>
void nforder(iter_type it, iter_type end) throw() {
  for (; it != end; it++) {
    if (UCS_UNPACK_CCC(*it) > 0) {
      iter_type start = it;
      for (it++; it != end && UCS_UNPACK_CCC(*it) > 0; it++) {
        // do nothing
      }
      // since combining class is in upper 8 bits,
      // this will sort by combining class
      stable_sort(start, it, nfcmpccc);
      it--;
    }
  }
}
#endif

#if !defined(UCS_PLAY_DUMB)
DPtr<uint32_t> *nfdecompose(const DPtr<uint32_t> *codepoints,
    const bool use_compat)
    THROWS (InvalidCodepointException, SizeUnknownException,
    BadAllocException, bad_alloc) {
  if (!codepoints->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  const uint32_t *begin = codepoints->dptr();
  const uint32_t *end = begin + codepoints->size();
  vector<uint32_t> decomp;
  decomp.reserve(codepoints->size());
  for (; begin != end; ++begin) {
    const uint32_t *d = nflookupd(*begin);
    if (d == NULL) {
      decomp.push_back(*begin);
      continue;
    }
    if (use_compat) {
      const uint32_t len = UCS_DECOMP_COMPAT_LEN(d);
      const uint32_t *c = UCS_DECOMP_COMPAT_CHARS(d);
      decomp.insert(decomp.end(), c, c + len);
    } else {
      const uint32_t len = UCS_DECOMP_CANON_LEN(d);
      const uint32_t *c = UCS_DECOMP_CANON_CHARS(d);
      decomp.insert(decomp.end(), c, c + len);
    }
  }
  nforder<vector<uint32_t>::iterator >(decomp.begin(), decomp.end());
  DPtr<uint32_t> *p;
  NEW(p, MPtr<uint32_t>, decomp.size());
  copy(decomp.begin(), decomp.end(), p->dptr());
  return p;
}
TRACE(BadAllocException, "Couldn't allocate memory for UCS decomposition.")
#endif

#if !defined(UCS_PLAY_DUMB)
uint8_t nfqc(const DPtr<uint32_t> *codepoints, size_t *pos, bool do_c, bool do_k)
    throw (InvalidCodepointException, SizeUnknownException) {
  if (!codepoints->sizeKnown()) {
    THROWX(SizeUnknownException);
  }
  uint8_t lastccc = 0;
  uint8_t result = UCS_QC_YES;
  const uint32_t *begin = codepoints->dptr();
  const uint32_t *end = begin + codepoints->size();
  for (; begin != end; ++begin) {
    const uint32_t *d = nflookupd(*begin);
    uint8_t ccc;
    if (d == NULL) {
      if (!nfvalid(*begin)) {
        THROW(InvalidCodepointException, *begin);
      }
      ccc = 0;
    } else {
      ccc = (uint8_t) UCS_DECOMP_CCC(d);
    }
    if (lastccc > ccc && ccc != UINT8_C(0)) {
      if (pos != NULL && result == UCS_QC_YES) {
        *pos = begin - codepoints->dptr();
      }
      return UCS_QC_NO;
    }
    uint8_t check;
    if (d == NULL) {
      check = UCS_QC_YES;
    } else {
      if (do_c) {
        if (do_k) {
          check = (uint8_t) UCS_DECOMP_NFKC_QC(d);
        } else {
          check = (uint8_t) UCS_DECOMP_NFC_QC(d);
        }
      } else {
        if (do_k) {
          check = (uint8_t) UCS_DECOMP_NFKD_QC(d);
        } else {
          check = (uint8_t) UCS_DECOMP_NFD_QC(d);
        }
      }
    }
    if (check == UCS_QC_NO) {
      if (pos != NULL && result == UCS_QC_YES) {
        *pos = begin - codepoints->dptr();
      }
      return UCS_QC_NO;
    }
    if (check == UCS_QC_MAYBE) {
      if (pos != NULL && result == UCS_QC_YES) {
        *pos = begin - codepoints->dptr();
      }
      result = UCS_QC_MAYBE;
    }
    lastccc = ccc;
  }
  return result;
}
#endif

#if !defined(UCS_PLAY_DUMB)
uint8_t nfd_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw(InvalidCodepointException, SizeUnknownException) {
  try {
    return nfqc(codepoints, pos, false, false);
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB)
DPtr<uint32_t> *nfd(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    return nfreturn(nfdecompose(codepoints, false));
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C)
DPtr<uint32_t> *nfcompose(const DPtr<uint32_t> *codepoints, const bool compat)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException);
#endif

#if !defined(UCS_PLAY_DUMB)
DPtr<uint32_t> *nfopt(DPtr<uint32_t> *codepoints, bool use_c, bool use_k)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    uint8_t qc = UCS_QC_YES;
    vector<size_t> bounds;
    bool reorder = false;
    uint32_t *begin = codepoints->dptr();
    uint32_t *end = begin + codepoints->size();
    uint32_t *mark = begin;
    for (; mark != end; ++mark) {
      if (*mark <= UINT32_C(0x7F)) {
        // ASCII doesn't need a lookup
        continue;
      }
      const uint32_t *d = nflookupd(*mark);
      if (d == NULL && !nfvalid(*mark)) {
        THROW(InvalidCodepointException, *mark);
      }
      uint32_t ccc = d == NULL ? 0 : UCS_DECOMP_CCC(d);
      reorder = reorder ||
          (mark != begin && UCS_UNPACK_CCC(*(mark - 1)) > ccc && ccc > 0);
      *mark = UCS_PACK(ccc, *mark);
      uint8_t q = d == NULL ? UCS_QC_YES :
          (use_c ? (use_k ? UCS_DECOMP_NFKC_QC(d) : UCS_DECOMP_NFC_QC(d))
                 : (use_k ? UCS_DECOMP_NFKD_QC(d) : UCS_DECOMP_NFD_QC(d)));
      if (q != UCS_QC_YES) {
        #if !defined(UCS_NO_C)
        if (use_c) {
          return nfcompose(nfreturn(codepoints), use_k);
        }
        #endif
        if (qc == UCS_QC_YES) {
          bounds.push_back(mark == begin ? mark - begin : mark - begin - 1);
          qc = q;
        }
      } else {
        if (qc != UCS_QC_YES) {
          bounds.push_back(mark - begin);
          qc = q;
        }
      }
    }
    if (bounds.size() == 0) {
      if (!reorder) {
        codepoints->hold();
        return codepoints;
      }
      MPtr<uint32_t> *reordered;
      NEW(reordered, MPtr<uint32_t>, codepoints->size());
      memcpy(reordered->dptr(), codepoints->dptr(),
          codepoints->size() * sizeof(uint32_t));
      nforder<uint32_t*>(reordered->dptr(),
          reordered->dptr() + reordered->size());
      nfreturn(codepoints);
      return reordered;
    }
    if (qc != UCS_QC_YES) {
      bounds.push_back(mark - begin);
    }
    vector<DPtr<uint32_t> *> norms;
    norms.reserve(bounds.size() >> 1);
    size_t total_len = 0;
    size_t i;
    for (i = 0; i < bounds.size(); i += 2) {
      if (i == 0) {
        total_len = bounds[i];
      } else {
        total_len += bounds[i] - bounds[i-1];
      }
      size_t len = bounds[i+1] - bounds[i];
      DPtr<uint32_t> *part = codepoints->sub(bounds[i], len);
      DPtr<uint32_t> *p;
      p = nfdecompose(nfreturn(part), use_k);
      part->drop();
      total_len += p->size();
      norms.push_back(p);
    }
    total_len += codepoints->size() - bounds[bounds.size() - 1];
    DPtr<uint32_t> *result;
    NEW(result, MPtr<uint32_t>, total_len);
    total_len = 0;
    for (i = 0; i < bounds.size(); i += 2) {
      if (i == 0) {
        memcpy(result->ptr(), codepoints->dptr(), bounds[i]*sizeof(uint32_t));
        total_len += bounds[i];
      } else {
        memcpy(result->dptr() + total_len, codepoints->dptr() + bounds[i-1],
            (bounds[i] - bounds[i-1]) * sizeof(uint32_t));
        total_len += bounds[i] - bounds[i-1];
      }
      size_t halfi = i >> 1;
      memcpy(result->dptr() + total_len, norms[halfi]->dptr(),
          norms[halfi]->size() * sizeof(uint32_t));
      total_len += norms[halfi]->size();
    }
    if (bounds[bounds.size() - 1] < codepoints->size()) {
      memcpy(result->dptr() + total_len,
          codepoints->dptr() + bounds[bounds.size() - 1],
          (codepoints->size() - bounds[bounds.size() - 1]) *sizeof(uint32_t));
    }
    for (i = 0; i < norms.size(); i++) {
      DPtr<uint32_t> *p = norms[i];
      norms[i] = NULL;
      p->drop();
    }
    nforder<uint32_t*>(result->dptr(), result->dptr() + result->size());
    nfreturn(codepoints);
    return nfreturn(result);
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB)
DPtr<uint32_t> *nfd_opt(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    return nfreturn(nfopt(codepoints, false, false));
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_K)
uint8_t nfkd_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw(InvalidCodepointException, SizeUnknownException) {
  try {
    return nfqc(codepoints, pos, false, true);
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_K)
DPtr<uint32_t> *nfkd(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    return nfreturn(nfdecompose(codepoints, true));
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_K)
DPtr<uint32_t> *nfkd_opt(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    return nfreturn(nfopt(codepoints, false, true));
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C)
DPtr<uint32_t> *nfcompose(const DPtr<uint32_t> *codepoints,
    const bool use_compat)
    THROWS(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  DPtr<uint32_t> *comp = nfdecompose(codepoints, use_compat);
  if (comp->size() == 0) {
    return comp;
  }
  try {
    size_t i;
    size_t newsize = 1;
    uint32_t *cpp = comp->dptr();
    for (i = 1; i < comp->size(); i++) {
      uint32_t cp = cpp[i];
      uint8_t ccc = UCS_UNPACK_CCC(cp);
      cp = UCS_UNPACK_CODEPOINT(cp);
      bool starter_found = false;
      uint32_t starter;
      size_t starti = 0;
      signed long j;
      for (j = newsize - 1; j >= 0; j--) {
        uint32_t cp2 = cpp[j];
        uint8_t ccc2 = UCS_UNPACK_CCC(cp2);
        cp2 = UCS_UNPACK_CODEPOINT(cp2);
        if (ccc2 == 0) {
          starter = cp2;
          starti = j;
          starter_found = true;
          break;
        }
        if (ccc2 >= ccc) { // blocked
          break;
        }
      }
      if (!starter_found) {
        cpp[newsize] = cpp[i];
        newsize++;
        continue;
      }
      const uint64_t pair = (((uint64_t) starter) << 32) | ((uint64_t) cp);
      const uint64_t *lb = lower_bound(UCS_COMPOSITION_INDEX,
          UCS_COMPOSITION_INDEX + UCS_COMPOSITION_INDEX_LEN, pair);
      if (lb == UCS_COMPOSITION_INDEX + UCS_COMPOSITION_INDEX_LEN || *lb != pair) {
        cpp[newsize] = cpp[i];
        newsize++;
        continue;
      }
      uint32_t offset = lb - UCS_COMPOSITION_INDEX;
      cpp[starti] = UCS_COMPOSITIONS[offset];
    }
    DPtr<uint32_t> *ret = comp->sub(0, newsize);
    comp->drop();
    return ret;
  } catch (BadAllocException &e) {
    comp->drop();
    RETHROW(e, "(rethrow)");
  }
}
TRACE(BadAllocException, "Couldn't allocate memory for UCS composition.")
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C)
uint8_t nfc_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw(InvalidCodepointException, SizeUnknownException) {
  try {
    return nfqc(codepoints, pos, true, false);
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C)
DPtr<uint32_t> *nfc(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    return nfreturn(nfcompose(codepoints, false));
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C)
DPtr<uint32_t> *nfc_opt(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    return nfreturn(nfopt(codepoints, true, false));
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C) && !defined(UCS_NO_K)
uint8_t nfkc_qc(const DPtr<uint32_t> *codepoints, size_t *pos)
    throw(InvalidCodepointException, SizeUnknownException) {
  try {
    return nfqc(codepoints, pos, true, true);
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C) && !defined(UCS_NO_K)
DPtr<uint32_t> *nfkc(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    return nfreturn(nfcompose(codepoints, true));
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

#if !defined(UCS_PLAY_DUMB) && !defined(UCS_NO_C) && !defined(UCS_NO_K)
DPtr<uint32_t> *nfkc_opt(DPtr<uint32_t> *codepoints)
    throw(InvalidCodepointException, SizeUnknownException,
    BadAllocException) {
  try {
    return nfreturn(nfopt(codepoints, true, true));
  } JUST_RETHROW(InvalidCodepointException, "(rethrow)")
    JUST_RETHROW(SizeUnknownException, "(rethrow)")
    JUST_RETHROW(BadAllocException, "(rethrow)")
}
#endif

}
