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

#include <djvAV/DPXSave.h>

#include <djvAV/OpenGLImage.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace AV
    {
        DPXSave::DPXSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo, const DPX::Options & options, const QPointer<Core::CoreContext> & context) :
            Save(fileInfo, ioInfo, context),
            _options(options)
        {
            if (_ioInfo.sequence.frames.count() > 1)
            {
                _fileInfo.setType(Core::FileInfo::SEQUENCE);
            }

            _info = PixelDataInfo();
            _info.size = _ioInfo.layers[0].size;
            _info.mirror.y = true;

            switch (_options.endian)
            {
            case DPX::ENDIAN_AUTO: break;
            case DPX::ENDIAN_MSB:  _info.endian = Core::Memory::MSB; break;
            case DPX::ENDIAN_LSB:  _info.endian = Core::Memory::LSB; break;
            default: break;
            }

            switch (_options.type)
            {
            case DPX::TYPE_U10: _info.pixel = Pixel::RGB_U10; break;
            default:
            {
                Pixel::TYPE type = Pixel::type(_ioInfo.layers[0].pixel);
                switch (type)
                {
                case Pixel::F16:
                case Pixel::F32: type = Pixel::U16; break;
                default: break;
                }
                _info.pixel = Pixel::pixel(Pixel::format(_ioInfo.layers[0].pixel), type);
            }
            break;
            }

            switch (Pixel::bitDepth(_info.pixel))
            {
            case 8:
            case 10: _info.align = 4; break;
            }
            //DJV_DEBUG_PRINT("info = " << _info);

            _image.set(_info);
        }

        DPXSave::~DPXSave()
        {}

        void DPXSave::write(const Image & in, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("DPXSave::write");
            //DJV_DEBUG_PRINT("in = " << in);

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
            _header = DPXHeader();
            _header.save(
                io,
                _info,
                _options.endian,
                _options.outputColorProfile,
                _options.version);

            // Convert the image.
            const PixelData * p = &in;
            if (in.info() != _info ||
                in.colorProfile.type != ColorProfile::RAW ||
                colorProfile.type != ColorProfile::RAW)
            {
                //DJV_DEBUG_PRINT("convert = " << _image);
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
