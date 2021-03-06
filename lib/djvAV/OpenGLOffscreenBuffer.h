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

#include <djvAV/OpenGL.h>
#include <djvAV/PixelData.h>

#include <djvCore/Error.h>

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

namespace djv
{
    namespace AV
    {
        //! This class provides an OpenGL offscreen buffer.
        class OpenGLOffscreenBuffer
        {
        public:
            //!
            //! Throws:
            //! - Core::Error
            OpenGLOffscreenBuffer(const PixelDataInfo &);
            ~OpenGLOffscreenBuffer();

            //! Get the offscreen buffer information.
            const PixelDataInfo & info() const;

            //! Get the offscreen buffer ID.
            GLuint id() const;

            //! Get the offscreen buffer texture ID.
            GLuint texture() const;

            //! Bind the offscreen buffer.
            void bind();

            //! Unbind the offscreen buffer.
            void unbind();

            //! This enumeration provides error codes.
            enum ERROR
            {
                ERROR_CREATE_TEXTURE,
                ERROR_INIT_TEXTURE,
                ERROR_CREATE_FBO,
                ERROR_INIT_FBO,

                ERROR_COUNT
            };

            //! Get the error code labels.
            static const QStringList & errorLabels();

        private:
            DJV_PRIVATE_COPY(OpenGLOffscreenBuffer);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides automatic binding for an OpenGL offscreen buffer.
        class OpenGLOffscreenBufferScope
        {
        public:
            OpenGLOffscreenBufferScope(OpenGLOffscreenBuffer *);
            ~OpenGLOffscreenBufferScope();

        private:
            DJV_PRIVATE_COPY(OpenGLOffscreenBufferScope);

            OpenGLOffscreenBuffer * _buffer = nullptr;
        };

    } // namespace AV
} // namespace djv
