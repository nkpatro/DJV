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

#include <djvAV/Color.h>
#include <djvAV/ColorProfile.h>
#include <djvAV/OpenGL.h>
#include <djvAV/PixelData.h>

#include <djvCore/Box.h>
#include <djvCore/Error.h>
#include <djvCore/Matrix.h>

#include <QMetaType>

class QPixmap;

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

namespace djv
{
    namespace AV
    {
        class OpenGLLUT;
        class OpenGLOffscreenBuffer;
        class OpenGLTexture;
        class OpenGLShader;

        //! This struct provides OpenGL image transform options.
        struct OpenGLImageXform
        {
            PixelDataInfo::Mirror mirror;
            glm::vec2             position = glm::vec2(0.f, 0.f);
            glm::vec2             scale    = glm::vec2(1.f, 1.f);
            float                 rotate   = 0.f; //! Rotation is measured in degrees.

            //! Create a transform matrix.
            static glm::mat4x4 xformMatrix(const OpenGLImageXform &);
        };

        //! This struct provides OpenGL image color options.
        struct OpenGLImageColor
        {
            float brightness = 1.f;
            float contrast   = 1.f;
            float saturation = 1.f;

            //! Create a brightness matrix.
            static glm::mat4x4 brightnessMatrix(float r, float g, float b);

            //! Create a contrast matrix.
            static glm::mat4x4 contrastMatrix(float r, float g, float b);

            //! Create a saturation matrix.
            static glm::mat4x4 saturationMatrix(float r, float g, float b);

            //! Create a color matrix.
            static glm::mat4x4 colorMatrix(const OpenGLImageColor &);
        };

        //! This struct provides OpenGL image color levels options.
        struct OpenGLImageLevels
        {
            float inLow   = 0.f;
            float inHigh  = 1.f;
            float gamma   = 1.f;
            float outLow  = 0.f;
            float outHigh = 1.f;

            //! Create a lookup table from color levels.
            static PixelData colorLut(const OpenGLImageLevels &, float softClip);
        };

        //! This struct provides OpenGL image display profile options.
        struct OpenGLImageDisplayProfile
        {
            PixelData         lut;
            OpenGLImageColor  color;
            OpenGLImageLevels levels;
            float             softClip = 0.f;
        };

        //! This class provides OpenGL image filtering options.
        class OpenGLImageFilter
        {
            Q_GADGET

        public:
            //! This enumeration provides the image filters.
            enum FILTER
            {
                NEAREST,
                LINEAR,
                BOX,
                TRIANGLE,
                BELL,
                BSPLINE,
                LANCZOS3,
                CUBIC,
                MITCHELL,

                FILTER_COUNT
            };
            Q_ENUM(FILTER);

            //! Get the image filter labels.
            static const QStringList & filterLabels();

            OpenGLImageFilter();
            OpenGLImageFilter(FILTER min, FILTER mag);

            FILTER min;
            FILTER mag;

            //! Convert an image filter to OpenGL.
            static GLenum toGl(FILTER);

            //! Get the default image filter.
            static OpenGLImageFilter filterDefault();

            //! Get the high quality image filter.
            static const OpenGLImageFilter & filterHighQuality();

            //! Get the global image filter.
            static const OpenGLImageFilter & filter();

            //! Set the global image filter.
            static void setFilter(const OpenGLImageFilter &);
        };

        //! This class provides OpenGL image options.
        class OpenGLImageOptions
        {
            Q_GADGET

        public:
            //! This enumeration provides the channels to display.
            enum CHANNEL
            {
                CHANNEL_DEFAULT,
                CHANNEL_RED,
                CHANNEL_GREEN,
                CHANNEL_BLUE,
                CHANNEL_ALPHA,

                CHANNEL_COUNT
            };
            Q_ENUM(CHANNEL);

            //! Get the channel labels.
            static const QStringList & channelLabels();

            OpenGLImageXform          xform;
            bool                      premultipliedAlpha = true;
            ColorProfile              colorProfile;
            OpenGLImageDisplayProfile displayProfile;
            CHANNEL                   channel = CHANNEL_DEFAULT;
            OpenGLImageFilter         filter;
            Color                     background;
            bool                      proxyScale = true;
        };

        //! This class provides a mesh for drawing images.
        class OpenGLImageMesh
        {
        public:
            OpenGLImageMesh();
            ~OpenGLImageMesh();

            void setSize(const glm::ivec2&, const PixelDataInfo::Mirror & mirror = PixelDataInfo::Mirror(), int proxyScale = 1);
            void draw();

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides OpenGL image utilities.
        class OpenGLImage
        {
        public:
            OpenGLImage();
            ~OpenGLImage();

            //! Draw pixel data.
            //!
            //! Throws:
            //! - Core::Error
            void draw(
                const PixelData &          data,
                const glm::mat4x4&         viewMatrix,
                const OpenGLImageOptions & options = OpenGLImageOptions(),
                Pixel::FORMAT              outputFormat = Pixel::RGBA);

            //! Copy pixel data.
            //!
            //! Throws:
            //! - Core::Error
            void copy(
                const PixelData &          input,
                PixelData &                output,
                const OpenGLImageOptions & options = OpenGLImageOptions());

            //! Setup OpenGL state for image drawing.
            static void stateUnpack(
                const PixelDataInfo & info,
                const glm::ivec2 &    offset = glm::ivec2(0, 0));

            //! Setup OpenGL state for image reading.
            static void statePack(
                const PixelDataInfo & info,
                const glm::ivec2 &    offset = glm::ivec2(0, 0));

            //! Calculate the average color.
            //!
            //! \todo Use a GPU implementation.
            //!
            //! Throws:
            //! - Core::Error
            void average(
                const PixelData &   input,
                Color &             output,
                const Pixel::Mask & mask = Pixel::Mask());

            //! Calculate the histogram.
            //!
            //! \todo Use a GPU implementation.
            //!
            //! Throws:
            //! - Core::Error
            void histogram(
                const PixelData &   input,
                PixelData &         output,
                int                 size,
                Color &             min,
                Color &             max,
                const Pixel::Mask & mask = Pixel::Mask());

            //! Convert pixel data to Qt.    
            QPixmap toQt(
                const PixelData &          pixelData,
                const OpenGLImageOptions & options = OpenGLImageOptions());

            //! This enumeration provides error codes.
            enum ERROR
            {
                ERROR_CREATE_TEXTURE,
                ERROR_CREATE_TEXTURE2,

                ERROR_COUNT
            };

            //! Get the error code labels.
            static const QStringList & errorLabels();

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace AV

    DJV_COMPARISON_OPERATOR(AV::OpenGLImageXform);
    DJV_COMPARISON_OPERATOR(AV::OpenGLImageColor);
    DJV_COMPARISON_OPERATOR(AV::OpenGLImageLevels);
    DJV_COMPARISON_OPERATOR(AV::OpenGLImageDisplayProfile);
    DJV_COMPARISON_OPERATOR(AV::OpenGLImageFilter);
    DJV_COMPARISON_OPERATOR(AV::OpenGLImageOptions);

    DJV_STRING_OPERATOR(AV::OpenGLImageXform);
    DJV_STRING_OPERATOR(AV::OpenGLImageColor);
    DJV_STRING_OPERATOR(AV::OpenGLImageLevels);
    DJV_STRING_OPERATOR(AV::OpenGLImageFilter);
    DJV_STRING_OPERATOR(AV::OpenGLImageFilter::FILTER);
    DJV_STRING_OPERATOR(AV::OpenGLImageOptions::CHANNEL);

    DJV_DEBUG_OPERATOR(AV::OpenGLImageXform);
    DJV_DEBUG_OPERATOR(AV::OpenGLImageColor);
    DJV_DEBUG_OPERATOR(AV::OpenGLImageLevels);
    DJV_DEBUG_OPERATOR(AV::OpenGLImageDisplayProfile);
    DJV_DEBUG_OPERATOR(AV::OpenGLImageFilter);
    DJV_DEBUG_OPERATOR(AV::OpenGLImageFilter::FILTER);
    DJV_DEBUG_OPERATOR(AV::OpenGLImageOptions);
    DJV_DEBUG_OPERATOR(AV::OpenGLImageOptions::CHANNEL);

} // namespace djv
