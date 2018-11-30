//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace Core
    {
        //! This function provides an assert (use the DJV_ASSERT macro instead).
        void _assert(const char * file, int line);

    } // namespace Core
} // namespace djv

//! This macro provides private implementation members.
#define DJV_PRIVATE() \
    struct Private; \
    std::unique_ptr<Private> _p

//! This macro makes a class non-copyable.
#define DJV_NON_COPYABLE(name) \
    name(const name &) = delete; \
    name & operator = (const name &) = delete

//! This macro marks strings for extraction.
#define DJV_TEXT(arg) (arg)

//! This macro provides an assert.
#if defined(DJV_ASSERT)
#undef DJV_ASSERT
#define DJV_ASSERT(value) \
    if (!(value)) \
        djv::Core::_assert(__FILE__, __LINE__)
#else
#define DJV_ASSERT(value)
#endif