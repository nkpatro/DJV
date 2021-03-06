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

#include <djvAV/CineonSave.h>

#include <djvAV/CineonHeader.h>
#include <djvAV/OpenGLImage.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace AV
    {
        CineonSave::CineonSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo, const Cineon::Options & options, const QPointer<Core::CoreContext> & context) :
            Save(fileInfo, ioInfo, context),
            _options(options)
        {
            if (ioInfo.sequence.frames.count() > 1)
            {
                _fileInfo.setType(Core::FileInfo::SEQUENCE);
            }

            _info = PixelDataInfo();
            _info.size = ioInfo.layers[0].size;
            _info.pixel = Pixel::RGB_U10;
            _info.mirror.y = true;
            _info.endian = Core::Memory::MSB;
            _info.align = 4;

            //DJV_DEBUG_PRINT("info = " << _info);

            _image.set(_info);
        }

        CineonSave::~CineonSave()
        {}

        void CineonSave::write(const Image & in, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("CineonSave::write");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("frame = " << frame);

            // Set the color profile.
            ColorProfile colorProfile;
            if (Cineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile ||
                Cineon::COLOR_PROFILE_AUTO == _options.outputColorProfile)
            {
                //DJV_DEBUG_PRINT("color profile");
                colorProfile.type = ColorProfile::LUT;
                colorProfile.lut = Cineon::linearToFilmPrintLut(_options.outputFilmPrint);
            }

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame);
            //DJV_DEBUG_PRINT("file name = " << fileName);
            IOInfo info(_info);
            info.layers[0].fileName = fileName;
            info.tags = in.tags;
            Core::FileIO io;
            io.open(fileName, Core::FileIO::WRITE);
            _header = CineonHeader();
            _header.save(io, info, _options.outputColorProfile);

            // Convert.
            const PixelData * p = &in;
            if (in.info() != _info ||
                in.colorProfile.type != ColorProfile::RAW ||
                colorProfile.type != ColorProfile::RAW)
            {
                //Core::_DEBUG_PRINT("convert = " << _image);
                _image.zero();
                OpenGLImageOptions options;
                options.colorProfile = colorProfile;
                OpenGLImage().copy(*p, _image, options);
                p = &_image;
            }

            // Write the file.
            io.set(p->data(), p->dataByteCount());
            _header.saveEnd(io);
        }

    } // namespace AV
} // namespace djv
