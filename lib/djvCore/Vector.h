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

#pragma once

#include <djvCore/Debug.h>
#include <djvCore/PicoJSON.h>

#include <QMetaType>
#include <QStringList>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace djv
{
#if (GLM_VERSION >= 96)
    typedef glm::precision precision;
#else
    typedef glm::qualifier precision;
#endif

    picojson::value toJSON(const glm::ivec2 &);
    picojson::value toJSON(const glm::vec2 &);
    picojson::value toJSON(const glm::vec3 &);
    picojson::value toJSON(const glm::vec4 &);

    //! Throws:
    //! - Error
    void fromJSON(const picojson::value &, glm::ivec2 &);
    void fromJSON(const picojson::value &, glm::vec2 &);
    void fromJSON(const picojson::value &, glm::vec3 &);
    void fromJSON(const picojson::value &, glm::vec4 &);

    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator >> (QStringList &, glm::tvec2<T, P> &);
    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator >> (QStringList &, glm::tvec3<T, P> &);
    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator >> (QStringList &, glm::tvec4<T, P> &);

    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator << (QStringList &, const glm::tvec2<T, P> &);
    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator << (QStringList &, const glm::tvec3<T, P> &);
    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator << (QStringList &, const glm::tvec4<T, P> &);

    template <typename T, precision P = glm::defaultp>
    inline Core::Debug & operator << (Core::Debug &, const glm::tvec2<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Debug & operator << (Core::Debug &, const glm::tvec3<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Debug & operator << (Core::Debug &, const glm::tvec4<T, P> &);

} // namespace djv

Q_DECLARE_METATYPE(glm::ivec2)
Q_DECLARE_METATYPE(glm::ivec3)
Q_DECLARE_METATYPE(glm::ivec4)
Q_DECLARE_METATYPE(glm::vec2)
Q_DECLARE_METATYPE(glm::vec3)
Q_DECLARE_METATYPE(glm::vec4)

#include <djvCore/VectorInline.h>

