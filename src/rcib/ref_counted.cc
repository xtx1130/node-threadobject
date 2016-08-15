// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef _WIN32
#include <windows.h>
#else
#endif
#include "macros.h"
#include "ref_counted.h"

namespace base {

  namespace subtle {

#ifndef _WIN32

    inline int Barrier_AtomicIncrement(volatile int* ptr,
      int increment) {
        for (;;) {
          // Atomic exchange the old value with an incremented one.
          int old_value = *ptr;
          int new_value = old_value + increment;
          if (__sync_bool_compare_and_swap(ptr, old_value, new_value)) {
            // The exchange took place as expected.
            return new_value;
          }
          // Otherwise, *ptr changed mid-loop and we need to retry.
        }
    }

#endif

    RefCountedThreadSafeBase::RefCountedThreadSafeBase() : ref_count_(0) {

    }

    RefCountedThreadSafeBase::~RefCountedThreadSafeBase() {
    }

    void RefCountedThreadSafeBase::AddRef() const {
#ifdef _WIN32
      InterlockedExchangeAdd(
        reinterpret_cast<volatile LONG*>(&ref_count_),
        static_cast<LONG>(1));
#else
      Barrier_AtomicIncrement(reinterpret_cast<volatile int*>(&ref_count_),
        static_cast<int>(1));
#endif
    }

    bool RefCountedThreadSafeBase::Release() const {
#ifdef _WIN32
      if (0 == (InterlockedExchangeAdd(
        reinterpret_cast<volatile LONG*>(&ref_count_),
        static_cast<LONG>(-1)) - 1))
        return true;
#else
      if(0 == Barrier_AtomicIncrement(reinterpret_cast<volatile int*>(&ref_count_),
        static_cast<int>(-1)))
        return true;
#endif
      return false;
    }

    bool RefCountedThreadSafeBase::HasOneRef() const {
      return (ref_count_ == 1);
    }

  }  // namespace subtle

}  // namespace base
