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

#include <djvAV/AVContext.h>
#include <djvAV/Image.h>
#include <djvAV/OpenGLImage.h>

#include <djvCore/FileInfo.h>

namespace djv
{
    namespace convert
    {
        //! This struct provides conversion options.
        struct Options
        {
            Options();

            AV::PixelDataInfo::Mirror mirror;
            glm::vec2 scale = glm::vec2(1.f, 1.f);
            AV::OpenGLImageOptions::CHANNEL channel;
            glm::ivec2 size = glm::ivec2(0, 0);
            Core::Box2i crop;
            Core::Box2f cropPercent;
        };

        //! This struct provides input options.
        struct Input
        {
            Input();

            Core::FileInfo file;
            size_t layer = 0;
            AV::PixelDataInfo::PROXY proxy;
            QString start;
            QString end;
            Core::FileInfo slate;
            int slateFrames;
            int timeout;
        };

        //! This struct provides output options.
        struct Output
        {
            Output();

            Core::FileInfo file;
            QScopedPointer<AV::Pixel::PIXEL> pixel;
            AV::Tags tags;
            bool tagsAuto;
            QScopedPointer<Core::Speed::FPS> speed;
        };

        //! This class provides global functionality for the application.
        class Context : public AV::AVContext
        {
            Q_OBJECT

        public:
            explicit Context(int & argc, char ** argv, QObject * parent = nullptr);
            ~Context() override;

            //! Get the options.
            const Options & options() const;

            //! Get the input options.
            const Input & input() const;

            //! Get the output options.
            const Output & output() const;

        protected:
            bool commandLineParse(QStringList &) override;
            QString commandLineHelp() const override;

        private:
            Options _options;
            Input _input;
            Output _output;
        };

    } // namespace convert
} // namespace djv
