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

#include <djvCoreTest/BoxTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Box.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>
#include <djvCore/StringUtil.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void BoxTest::run(int &, char **)
        {
            DJV_DEBUG("BoxTest::run");
            ctors();
            members();
            operators();
        }

        void BoxTest::ctors()
        {
            DJV_DEBUG("BoxTest::ctors");
            {
                const Box2i box;
                DJV_ASSERT(0 == box.position.x);
                DJV_ASSERT(0 == box.position.y);
                DJV_ASSERT(0 == box.size.x);
                DJV_ASSERT(0 == box.size.y);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(0 == box.w);
                DJV_ASSERT(0 == box.h);
            }
            {
                const Box3i box;
                DJV_ASSERT(0 == box.position.x);
                DJV_ASSERT(0 == box.position.y);
                DJV_ASSERT(0 == box.position.z);
                DJV_ASSERT(0 == box.size.x);
                DJV_ASSERT(0 == box.size.y);
                DJV_ASSERT(0 == box.size.z);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(0 == box.z);
                DJV_ASSERT(0 == box.w);
                DJV_ASSERT(0 == box.h);
                DJV_ASSERT(0 == box.d);
            }
            {
                const Box2i tmp(1, 2, 3, 4);
                const Box2i box(tmp);
                DJV_ASSERT(1 == box.position.x);
                DJV_ASSERT(2 == box.position.y);
                DJV_ASSERT(3 == box.size.x);
                DJV_ASSERT(4 == box.size.y);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(4 == box.h);
            }
            {
                const Box3i tmp(1, 2, 3, 4, 5, 6);
                const Box3i box(tmp);
                DJV_ASSERT(1 == box.position.x);
                DJV_ASSERT(2 == box.position.y);
                DJV_ASSERT(3 == box.position.z);
                DJV_ASSERT(4 == box.size.x);
                DJV_ASSERT(5 == box.size.y);
                DJV_ASSERT(6 == box.size.z);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.z);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
                DJV_ASSERT(6 == box.d);
            }
            {
                const glm::ivec2 tmp(1, 2);
                const Box2i box(tmp);
                DJV_ASSERT(0 == box.position.x);
                DJV_ASSERT(0 == box.position.y);
                DJV_ASSERT(1 == box.size.x);
                DJV_ASSERT(2 == box.size.y);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(1 == box.w);
                DJV_ASSERT(2 == box.h);
            }
            {
                const glm::ivec3 tmp(1, 2, 3);
                const Box3i box(tmp);
                DJV_ASSERT(0 == box.position.x);
                DJV_ASSERT(0 == box.position.y);
                DJV_ASSERT(0 == box.position.z);
                DJV_ASSERT(1 == box.size.x);
                DJV_ASSERT(2 == box.size.y);
                DJV_ASSERT(3 == box.size.z);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(0 == box.z);
                DJV_ASSERT(1 == box.w);
                DJV_ASSERT(2 == box.h);
                DJV_ASSERT(3 == box.d);
            }
            {
                const Box2i box(glm::ivec2(1, 2), glm::ivec2(3, 4));
                DJV_ASSERT(1 == box.position.x);
                DJV_ASSERT(2 == box.position.y);
                DJV_ASSERT(3 == box.size.x);
                DJV_ASSERT(4 == box.size.y);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(4 == box.h);
            }
            {
                const Box2i box(1, 2);
                DJV_ASSERT(0 == box.position.x);
                DJV_ASSERT(0 == box.position.y);
                DJV_ASSERT(1 == box.size.x);
                DJV_ASSERT(2 == box.size.y);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(1 == box.w);
                DJV_ASSERT(2 == box.h);
            }
            {
                const Box3i box(1, 2, 3);
                DJV_ASSERT(0 == box.position.x);
                DJV_ASSERT(0 == box.position.y);
                DJV_ASSERT(0 == box.position.z);
                DJV_ASSERT(1 == box.size.x);
                DJV_ASSERT(2 == box.size.y);
                DJV_ASSERT(3 == box.size.z);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(0 == box.z);
                DJV_ASSERT(1 == box.w);
                DJV_ASSERT(2 == box.h);
                DJV_ASSERT(3 == box.d);
            }
            {
                const Box2i box(glm::ivec2(1, 2), glm::ivec2(3, 4));
                DJV_ASSERT(1 == box.position.x);
                DJV_ASSERT(2 == box.position.y);
                DJV_ASSERT(3 == box.size.x);
                DJV_ASSERT(4 == box.size.y);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(4 == box.h);
            }
            {
                const Box3i box(glm::ivec3(1, 2, 3), glm::ivec3(4, 5, 6));
                DJV_ASSERT(1 == box.position.x);
                DJV_ASSERT(2 == box.position.y);
                DJV_ASSERT(3 == box.position.z);
                DJV_ASSERT(4 == box.size.x);
                DJV_ASSERT(5 == box.size.y);
                DJV_ASSERT(6 == box.size.z);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.z);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
                DJV_ASSERT(6 == box.d);
            }
            {
                const Box2i box(glm::ivec2(1, 2));
                DJV_ASSERT(0 == box.position.x);
                DJV_ASSERT(0 == box.position.y);
                DJV_ASSERT(1 == box.size.x);
                DJV_ASSERT(2 == box.size.y);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(1 == box.w);
                DJV_ASSERT(2 == box.h);
            }
            {
                const Box3i box(glm::ivec3(1, 2, 3));
                DJV_ASSERT(0 == box.position.x);
                DJV_ASSERT(0 == box.position.y);
                DJV_ASSERT(0 == box.position.z);
                DJV_ASSERT(1 == box.size.x);
                DJV_ASSERT(2 == box.size.y);
                DJV_ASSERT(3 == box.size.z);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(0 == box.z);
                DJV_ASSERT(1 == box.w);
                DJV_ASSERT(2 == box.h);
                DJV_ASSERT(3 == box.d);
            }
        }

        void BoxTest::members()
        {
            DJV_DEBUG("BoxTest::members");
            {
                Box2i box(1, 2, 3, 4);
                box.zero();
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(0 == box.w);
                DJV_ASSERT(0 == box.h);
            }
            {
                Box3i box(1, 2, 3, 4, 5, 6);
                box.zero();
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(0 == box.z);
                DJV_ASSERT(0 == box.w);
                DJV_ASSERT(0 == box.h);
                DJV_ASSERT(0 == box.d);
            }
            {
                Box2i box(1, 2, 3, 4);
                DJV_ASSERT(3 == box.lowerRight().x);
                DJV_ASSERT(5 == box.lowerRight().y);
                box.setLowerRight(glm::ivec2(4, 6));
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
            }
            {
                Box3i box(1, 2, 3, 4, 5, 6);
                DJV_ASSERT(4 == box.lowerRight().x);
                DJV_ASSERT(6 == box.lowerRight().y);
                DJV_ASSERT(8 == box.lowerRight().z);
                box.setLowerRight(glm::ivec3(5, 7, 9));
                DJV_ASSERT(5 == box.w);
                DJV_ASSERT(6 == box.h);
                DJV_ASSERT(7 == box.d);
            }
        }

        void BoxTest::operators()
        {
            DJV_DEBUG("BoxTest::operators");
            {
                const Box2i tmp(1, 2, 3, 4);
                Box2i box;
                box = tmp;
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(4 == box.h);
            }
            {
                const Box3i tmp(1, 2, 3, 4, 5, 6);
                Box3i box;
                box = tmp;
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.z);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
                DJV_ASSERT(6 == box.d);
            }
            {
                Box2i box(1, 2, 3, 4);
                box *= glm::ivec2(2, 3);
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(6 == box.y);
                DJV_ASSERT(6 == box.w);
                DJV_ASSERT(12 == box.h);
            }
            {
                Box3i box(1, 2, 3, 4, 5, 6);
                box *= glm::ivec3(2, 3, 4);
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(6 == box.y);
                DJV_ASSERT(12 == box.z);
                DJV_ASSERT(8 == box.w);
                DJV_ASSERT(15 == box.h);
                DJV_ASSERT(24 == box.d);
            }
            {
                Box2i box(4, 6, 8, 9);
                box /= glm::ivec2(2, 3);
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(3 == box.h);
            }
            {
                Box3i box(4, 6, 8, 10, 12, 16);
                box /= glm::ivec3(2, 3, 4);
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(2 == box.z);
                DJV_ASSERT(5 == box.w);
                DJV_ASSERT(4 == box.h);
                DJV_ASSERT(4 == box.d);
            }
            {
                Box2i box(4, 6, 8, 10);
                box /= 2;
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(3 == box.y);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
            }
            {
                Box3i box(4, 6, 8, 10, 12, 14);
                box /= 2;
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(3 == box.y);
                DJV_ASSERT(4 == box.z);
                DJV_ASSERT(5 == box.w);
                DJV_ASSERT(6 == box.h);
                DJV_ASSERT(7 == box.d);
            }
            {
                const Box2f box = Box2i(1, 2, 3, 4);
                DJV_ASSERT(Math::fuzzyCompare(1.f, box.x));
                DJV_ASSERT(Math::fuzzyCompare(2.f, box.y));
                DJV_ASSERT(Math::fuzzyCompare(3.f, box.w));
                DJV_ASSERT(Math::fuzzyCompare(4.f, box.h));
            }
            {
                const Box3f box = Box3i(1, 2, 3, 4, 5, 6);
                DJV_ASSERT(Math::fuzzyCompare(1.f, box.x));
                DJV_ASSERT(Math::fuzzyCompare(2.f, box.y));
                DJV_ASSERT(Math::fuzzyCompare(3.f, box.z));
                DJV_ASSERT(Math::fuzzyCompare(4.f, box.w));
                DJV_ASSERT(Math::fuzzyCompare(5.f, box.h));
                DJV_ASSERT(Math::fuzzyCompare(6.f, box.d));
            }
            {
                const Box2i box = Box2f(1.f, 2.f, 3.f, 4.f);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(4 == box.h);
            }
            {
                const Box3i box = Box3f(1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.z);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
                DJV_ASSERT(6 == box.d);
            }
            {
                const Box2i box = Box2i(1, 2, 3, 4) + 1;
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(3 == box.y);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
            }
            {
                const Box3i box = Box3i(1, 2, 3, 4, 5, 6) + 1;
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(3 == box.y);
                DJV_ASSERT(4 == box.z);
                DJV_ASSERT(5 == box.w);
                DJV_ASSERT(6 == box.h);
                DJV_ASSERT(7 == box.d);
            }
            {
                const Box2i box = Box2i(1, 2, 3, 4) - 1;
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(1 == box.y);
                DJV_ASSERT(2 == box.w);
                DJV_ASSERT(3 == box.h);
            }
            {
                const Box3i box = Box3i(1, 2, 3, 4, 5, 6) - 1;
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(1 == box.y);
                DJV_ASSERT(2 == box.z);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(4 == box.h);
                DJV_ASSERT(5 == box.d);
            }
            {
                const Box2i box = Box2i(1, 2, 3, 4) * 2;
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(4 == box.y);
                DJV_ASSERT(6 == box.w);
                DJV_ASSERT(8 == box.h);
            }
            {
                const Box3i box = Box3i(1, 2, 3, 4, 5, 6) * 2;
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(4 == box.y);
                DJV_ASSERT(6 == box.z);
                DJV_ASSERT(8 == box.w);
                DJV_ASSERT(10 == box.h);
                DJV_ASSERT(12 == box.d);
            }
            {
                const Box2i box = Box2i(2, 4, 6, 8) / 2;
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(4 == box.h);
            }
            {
                const Box3i box = Box3i(2, 4, 6, 8, 10, 12) / 2;
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.z);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
                DJV_ASSERT(6 == box.d);
            }

            {
                const Box2i box = Box2i(1, 2, 3, 4) + glm::ivec2(1, 2);
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(4 == box.y);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(6 == box.h);
            }
            {
                const Box3i box = Box3i(1, 2, 3, 4, 5, 6) + glm::ivec3(1, 2, 3);
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(4 == box.y);
                DJV_ASSERT(6 == box.z);
                DJV_ASSERT(5 == box.w);
                DJV_ASSERT(7 == box.h);
                DJV_ASSERT(9 == box.d);
            }
            {
                const Box2i box = Box2i(1, 2, 3, 4) - glm::ivec2(1, 2);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(2 == box.w);
                DJV_ASSERT(2 == box.h);
            }
            {
                const Box3i box = Box3i(1, 2, 3, 4, 5, 6) - glm::ivec3(1, 2, 3);
                DJV_ASSERT(0 == box.x);
                DJV_ASSERT(0 == box.y);
                DJV_ASSERT(0 == box.z);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(3 == box.h);
                DJV_ASSERT(3 == box.d);
            }
            {
                const Box2i box = Box2i(1, 2, 3, 4) * glm::ivec2(2, 3);
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(6 == box.y);
                DJV_ASSERT(6 == box.w);
                DJV_ASSERT(12 == box.h);
            }
            {
                const Box3i box = Box3i(1, 2, 3, 4, 5, 6) * glm::ivec3(2, 3, 4);
                DJV_ASSERT(2 == box.x);
                DJV_ASSERT(6 == box.y);
                DJV_ASSERT(12 == box.z);
                DJV_ASSERT(8 == box.w);
                DJV_ASSERT(15 == box.h);
                DJV_ASSERT(24 == box.d);
            }
            {
                const Box2i box = Box2i(2, 3, 4, 6) / glm::ivec2(2, 3);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(1 == box.y);
                DJV_ASSERT(2 == box.w);
                DJV_ASSERT(2 == box.h);
            }
            {
                const Box3i box = Box3i(2, 3, 4, 4, 6, 8) / glm::ivec3(2, 3, 4);
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(1 == box.y);
                DJV_ASSERT(1 == box.z);
                DJV_ASSERT(2 == box.w);
                DJV_ASSERT(2 == box.h);
                DJV_ASSERT(2 == box.d);
            }
            {
                DJV_ASSERT(Box2i(1, 2, 3, 4) == Box2i(1, 2, 3, 4));
                DJV_ASSERT(Box2i(1, 2, 3, 4) != Box2i());
            }
            {
                Box2i box;
                QStringList s = QStringList() << "1" << "2" << "3" << "4";
                s >> box;
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.w);
                DJV_ASSERT(4 == box.h);
                DJV_ASSERT(s.isEmpty());
            }
            {
                Box3i box;
                QStringList s = QStringList() << "1" << "2" << "3" << "4" << "5" << "6";
                s >> box;
                DJV_ASSERT(1 == box.x);
                DJV_ASSERT(2 == box.y);
                DJV_ASSERT(3 == box.z);
                DJV_ASSERT(4 == box.w);
                DJV_ASSERT(5 == box.h);
                DJV_ASSERT(6 == box.d);
                DJV_ASSERT(s.isEmpty());
            }
            {
                Box2i box(1, 2, 3, 4);
                QStringList s;
                s << box;
                DJV_ASSERT((QStringList() << "1" << "2" << "3" << "4") == s);
            }
            {
                Box3i box(1, 2, 3, 4, 5, 6);
                QStringList s;
                s << box;
                DJV_ASSERT((QStringList() << "1" << "2" << "3" << "4" << "5" << "6") == s);
            }
            {
                DJV_DEBUG_PRINT(Box2i());
                DJV_DEBUG_PRINT(Box2f());
                DJV_DEBUG_PRINT(Box3i());
                DJV_DEBUG_PRINT(Box3f());
            }
        }

    } // namespace CoreTest
} // namespace djv
