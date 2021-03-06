//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvCoreTest/DebugTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QStringList>
#include <QVector>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void DebugTest::run(int &, char **)
        {
            DJV_DEBUG("DebugTest::run");
            DJV_DEBUG_PRINT("string = " << QString("string"));
            DJV_DEBUG_PRINT("char * = " << "char *");
            DJV_DEBUG_PRINT("bool = " << true);
            DJV_DEBUG_PRINT("int = " << static_cast<int>(1));
            DJV_DEBUG_PRINT("unsigned int = " << static_cast<unsigned int>(1));
            DJV_DEBUG_PRINT("qint64 = " << static_cast<qint64>(1));
            DJV_DEBUG_PRINT("quint64 = " << static_cast<quint64>(1));
            DJV_DEBUG_PRINT("float = " << 1.f);
            DJV_DEBUG_PRINT("double = " << 1.0);
            DJV_DEBUG_PRINT("string list = " << QStringList() << "a" << "b" << "c");
            DJV_DEBUG_PRINT("bits = " << Debug::bitsU8(1));
            DJV_DEBUG_PRINT("bits = " << Debug::bitsU16(1));
            DJV_DEBUG_PRINT("bits = " << Debug::bitsU32(1));
        }

    } // namespace CoreTest
} // namespace djv
