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

#include <djvAV/DPXHeader.h>

#include <djvAV/CineonHeader.h>

#include <djvCore/Debug.h>
#include <djvCore/Time.h>

namespace djv
{
    namespace AV
    {
        const char DPXHeader::magic[][5] =
        {
            "SDPX",
            "XPDS"
        };

        DPXHeader::DPXHeader()
        {
            Core::Memory::fill<quint8>(0xff, &file, sizeof(File));
            zero(file.version, 8);
            zero(file.name, 100);
            zero(file.time, 24);
            zero(file.creator, 100);
            zero(file.project, 200);
            zero(file.copyright, 200);

            Core::Memory::fill<quint8>(0xff, &image, sizeof(Image));

            Core::Memory::fill<quint8>(0xff, &source, sizeof(Source));
            zero(source.file, 100);
            zero(source.time, 24);
            zero(source.inputDevice, 32);
            zero(source.inputSerial, 32);

            Core::Memory::fill<quint8>(0xff, &film, sizeof(Film));
            zero(film.id, 2);
            zero(film.type, 2);
            zero(film.offset, 2);
            zero(film.prefix, 6);
            zero(film.count, 4);
            zero(film.format, 32);
            zero(film.frameId, 32);
            zero(film.slate, 100);

            Core::Memory::fill<quint8>(0xff, &tv, sizeof(Tv));
        }

        void DPXHeader::load(Core::FileIO & io, IOInfo & info, bool & filmPrint)
        {
            //DJV_DEBUG("DPXHeader::load");
            //DJV_DEBUG_PRINT("file = " << io.fileName());

            // Read.
            io.get(&file, sizeof(File));
            //DJV_DEBUG_PRINT("magic = " << QString::fromLatin1((char *)&file.magic, 4));
            if (0 == memcmp(&file.magic, magic[0], 4))
            {
                info.layers[0].endian = Core::Memory::MSB;
            }
            else if (0 == memcmp(&file.magic, magic[1], 4))
            {
                info.layers[0].endian = Core::Memory::LSB;
            }
            else
            {
                throw Core::Error(
                    DPX::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNRECOGNIZED]);
            }
            //DJV_DEBUG_PRINT("endian = " << info.endian);
            io.get(&image, sizeof(Image));
            io.get(&source, sizeof(Source));
            io.get(&film, sizeof(Film));
            io.get(&tv, sizeof(Tv));
            if (info.layers[0].endian != Core::Memory::endian())
            {
                //DJV_DEBUG_PRINT("endian");
                io.setEndian(true);
                endian();
            }

            // Information.
            if (image.elemSize != 1)
            {
                throw Core::Error(
                    DPX::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }
            info.layers[0].size = glm::ivec2(image.size[0], image.size[1]);
            //DJV_DEBUG_PRINT("size = " << info.size);
            switch (image.orient)
            {
            case ORIENT_LEFT_RIGHT_TOP_BOTTOM:
                info.layers[0].mirror.y = true;
                break;
            case ORIENT_RIGHT_LEFT_TOP_BOTTOM:
                info.layers[0].mirror.x = true;
                break;
            case ORIENT_LEFT_RIGHT_BOTTOM_TOP:
                break;
            case ORIENT_RIGHT_LEFT_BOTTOM_TOP:
                info.layers[0].mirror.x = true;
                info.layers[0].mirror.y = true;
                break;
            case ORIENT_TOP_BOTTOM_LEFT_RIGHT:
            case ORIENT_TOP_BOTTOM_RIGHT_LEFT:
            case ORIENT_BOTTOM_TOP_LEFT_RIGHT:
            case ORIENT_BOTTOM_TOP_RIGHT_LEFT:
                break;
            }

            Pixel::PIXEL pixel = static_cast<Pixel::PIXEL>(0);
            bool found = false;
            switch (image.elem[0].packing)
            {
            case PACK:
            {
                //DJV_DEBUG_PRINT("pack");
                int channels = 0;
                switch (image.elem[0].descriptor)
                {
                case DESCRIPTOR_L:
                    channels = 1;
                    break;
                case DESCRIPTOR_RGB:
                    channels = 3;
                    break;
                case DESCRIPTOR_RGBA:
                    channels = 4;
                    break;
                }
                //DJV_DEBUG_PRINT("channels = " << channels);
                //DJV_DEBUG_PRINT("bit depth = " << image.elem[0].bitDepth);
                found = Pixel::intPixel(channels, image.elem[0].bitDepth, pixel);
            }
            break;
            case TYPE_A:
                //DJV_DEBUG_PRINT("type a");
                switch (image.elem[0].descriptor)
                {
                case DESCRIPTOR_RGB:
                    switch (image.elem[0].bitDepth)
                    {
                    case 10:
                        pixel = Pixel::RGB_U10;
                        found = true;
                        break;
                    case 16:
                    {
                        int channels = 0;
                        switch (image.elem[0].descriptor)
                        {
                        case DESCRIPTOR_L:
                            channels = 1;
                            break;
                        case DESCRIPTOR_RGB:
                            channels = 3;
                            break;
                        case DESCRIPTOR_RGBA:
                            channels = 4;
                            break;
                        }
                        //DJV_DEBUG_PRINT("channels = " << channels);
                        //DJV_DEBUG_PRINT("bit depth = " <<
                        //    image.elem[0].bitDepth);
                        found = Pixel::intPixel(channels, image.elem[0].bitDepth, pixel);
                    }
                    break;
                    }
                    break;
                }
                break;
            }
            if (!found)
            {
                throw Core::Error(
                    DPX::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            info.layers[0].pixel = pixel;
            //DJV_DEBUG_PRINT("pixel = " << info.pixel);
            switch (Pixel::bitDepth(info.layers[0].pixel))
            {
            case 8:
            case 10: info.layers[0].align = 4; break;
            }

            if (image.elem[0].encoding)
            {
                throw Core::Error(
                    DPX::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            if (isValid(&image.elem[0].linePadding) && image.elem[0].linePadding)
            {
                throw Core::Error(
                    DPX::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_UNSUPPORTED]);
            }

            filmPrint = TRANSFER_FILM_PRINT == image.elem[0].transfer;

            // File tags.
            const QStringList & tags = Tags::tagLabels();
            const QStringList & dpxTags = DPX::tagLabels();
            if (isValid(file.time, 24))
            {
                info.tags[tags[Tags::TIME]] = toString(file.time, 24);
            }
            if (isValid(file.creator, 100))
            {
                info.tags[tags[Tags::CREATOR]] = toString(file.creator, 100);
            }
            if (isValid(file.project, 200))
            {
                info.tags[tags[Tags::PROJECT]] = toString(file.project, 200);
            }
            if (isValid(file.copyright, 200))
            {
                info.tags[tags[Tags::COPYRIGHT]] = toString(file.copyright, 200);
            }

            // Source tags.
            if (isValid(&source.offset[0]) && isValid(&source.offset[1]))
                info.tags[dpxTags[DPX::TAG_SOURCE_OFFSET]] = (QStringList() <<
                    QString::number(source.offset[0]) <<
                    QString::number(source.offset[1])).join(" ");
            if (isValid(&source.center[0]) && isValid(&source.center[1]))
                info.tags[dpxTags[DPX::TAG_SOURCE_CENTER]] = (QStringList() <<
                    QString::number(source.center[0]) <<
                    QString::number(source.center[1])).join(" ");
            if (isValid(&source.size[0]) && isValid(&source.size[1]))
                info.tags[dpxTags[DPX::TAG_SOURCE_SIZE]] = (QStringList() <<
                    QString::number(source.size[0]) <<
                    QString::number(source.size[1])).join(" ");
            if (isValid(source.file, 100))
            {
                info.tags[dpxTags[DPX::TAG_SOURCE_FILE]] =
                    toString(source.file, 100);
            }
            if (isValid(source.time, 24))
            {
                info.tags[tags[DPX::TAG_SOURCE_TIME]] = toString(source.time, 24);
            }
            if (isValid(source.inputDevice, 32))
                info.tags[dpxTags[DPX::TAG_SOURCE_INPUT_DEVICE]] =
                toString(source.inputDevice, 32);
            if (isValid(source.inputSerial, 32))
                info.tags[dpxTags[DPX::TAG_SOURCE_INPUT_SERIAL]] =
                toString(source.inputSerial, 32);
            if (isValid(&source.border[0]) && isValid(&source.border[1]) &&
                isValid(&source.border[2]) && isValid(&source.border[3]))
                info.tags[dpxTags[DPX::TAG_SOURCE_BORDER]] = (QStringList() <<
                    QString::number(source.border[0]) <<
                    QString::number(source.border[1]) <<
                    QString::number(source.border[2]) <<
                    QString::number(source.border[3])).join(" ");
            if (isValid(&source.pixelAspect[0]) && isValid(&source.pixelAspect[1]))
                info.tags[dpxTags[DPX::TAG_SOURCE_PIXEL_ASPECT]] = (QStringList() <<
                    QString::number(source.pixelAspect[0]) <<
                    QString::number(source.pixelAspect[1])).join(" ");
            if (isValid(&source.scanSize[0]) && isValid(&source.scanSize[1]))
                info.tags[dpxTags[DPX::TAG_SOURCE_SCAN_SIZE]] = (QStringList() <<
                    QString::number(source.scanSize[0]) <<
                    QString::number(source.scanSize[1])).join(" ");

            // Film tags.
            if (
                isValid(film.id, 2) && isValid(film.type, 2) &&
                isValid(film.offset, 2) && isValid(film.prefix, 6) &&
                isValid(film.count, 4))
                info.tags[tags[Tags::KEYCODE]] = Core::Time::keycodeToString(
                    toString(film.id, 2).toInt(),
                    toString(film.type, 2).toInt(),
                    toString(film.prefix, 6).toInt(),
                    toString(film.count, 4).toInt(),
                    toString(film.offset, 2).toInt());
            if (isValid(film.format, 32))
            {
                info.tags[dpxTags[DPX::TAG_FILM_FORMAT]] =
                    toString(film.format, 32);
            }
            if (isValid(&film.frame))
                info.tags[dpxTags[DPX::TAG_FILM_FRAME]] =
                QString::number(film.frame);
            if (isValid(&film.sequence))
                info.tags[dpxTags[DPX::TAG_FILM_SEQUENCE]] =
                QString::number(film.sequence);
            if (isValid(&film.hold))
            {
                info.tags[dpxTags[DPX::TAG_FILM_HOLD]] =
                    QString::number(film.hold);
            }
            if (isValid(&film.frameRate) &&
                film.frameRate > CineonHeader::minSpeed)
            {
                info.sequence.speed = Core::Speed::floatToSpeed(film.frameRate);

                info.tags[dpxTags[DPX::TAG_FILM_FRAME_RATE]] =
                    QString::number(film.frameRate);
            }
            if (isValid(&film.shutter))
                info.tags[dpxTags[DPX::TAG_FILM_SHUTTER]] =
                QString::number(film.shutter);
            if (isValid(film.frameId, 32))
            {
                info.tags[dpxTags[DPX::TAG_FILM_FRAME_ID]] =
                    toString(film.frameId, 32);
            }
            if (isValid(film.slate, 100))
            {
                info.tags[dpxTags[DPX::TAG_FILM_SLATE]] = toString(film.slate, 100);
            }

            // TV tags.
            if (isValid(&tv.timecode))
                info.tags[tags[Tags::TIMECODE]] =
                Core::Time::timecodeToString(tv.timecode);
            if (isValid(&tv.interlace))
                info.tags[dpxTags[DPX::TAG_TV_INTERLACE]] =
                QString::number(tv.interlace);
            if (isValid(&tv.field))
            {
                info.tags[dpxTags[DPX::TAG_TV_FIELD]] =
                    QString::number(tv.field);
            }
            if (isValid(&tv.videoSignal))
                info.tags[dpxTags[DPX::TAG_TV_VIDEO_SIGNAL]] =
                QString::number(tv.videoSignal);
            if (isValid(&tv.sampleRate[0]) && isValid(&tv.sampleRate[1]))
                info.tags[dpxTags[DPX::TAG_TV_SAMPLE_RATE]] = (QStringList() <<
                    QString::number(tv.sampleRate[0]) <<
                    QString::number(tv.sampleRate[1])).join(" ");
            if (isValid(&tv.frameRate) &&
                tv.frameRate > CineonHeader::minSpeed)
            {
                info.sequence.speed = Core::Speed::floatToSpeed(tv.frameRate);
                info.tags[dpxTags[DPX::TAG_TV_FRAME_RATE]] =
                    QString::number(tv.frameRate);
            }
            if (isValid(&tv.timeOffset))
                info.tags[dpxTags[DPX::TAG_TV_TIME_OFFSET]] =
                QString::number(tv.timeOffset);
            if (isValid(&tv.gamma))
            {
                info.tags[dpxTags[DPX::TAG_TV_GAMMA]] =
                    QString::number(tv.gamma);
            }
            if (isValid(&tv.blackLevel))
                info.tags[dpxTags[DPX::TAG_TV_BLACK_LEVEL]] =
                QString::number(tv.blackLevel);
            if (isValid(&tv.blackGain))
                info.tags[dpxTags[DPX::TAG_TV_BLACK_GAIN]] =
                QString::number(tv.blackGain);
            if (isValid(&tv.breakpoint))
                info.tags[dpxTags[DPX::TAG_TV_BREAK_POINT]] =
                QString::number(tv.breakpoint);
            if (isValid(&tv.whiteLevel))
                info.tags[dpxTags[DPX::TAG_TV_WHITE_LEVEL]] =
                QString::number(tv.whiteLevel);
            if (isValid(&tv.integrationTimes))
                info.tags[dpxTags[DPX::TAG_TV_INTEGRATION_TIMES]] =
                QString::number(tv.integrationTimes);

            // End.
            debug();
            if (file.imageOffset)
            {
                io.setPos(file.imageOffset);
            }
        }

        void DPXHeader::save(
            Core::FileIO & io,
            const IOInfo & info,
            DPX::ENDIAN endian,
            Cineon::COLOR_PROFILE colorProfile,
            DPX::VERSION version)
        {
            //DJV_DEBUG("DPXHeader::save");

            // Information.
            switch (version)
            {
            case DPX::VERSION_1_0:
                memcpy(file.version, "V1.0", 4);
                break;
            case DPX::VERSION_2_0:
                memcpy(file.version, "V2.0", 4);
                break;
            default: break;
            }

            file.imageOffset = 2048;
            file.headerSize = 2048 - 384;
            file.industryHeaderSize = 384;
            file.userHeaderSize = 0;
            file.size = 0;
            file.dittoKey = 1;

            image.elemSize = 1;
            image.size[0] = info.layers[0].size.x;
            image.size[1] = info.layers[0].size.y;
            image.orient = ORIENT_LEFT_RIGHT_TOP_BOTTOM;

            switch (info.layers[0].pixel)
            {
            case Pixel::L_U8:
            case Pixel::L_U16:
            case Pixel::L_F16:
            case Pixel::L_F32:
                image.elem[0].descriptor = DESCRIPTOR_L;
                break;
            case Pixel::RGB_U8:
            case Pixel::RGB_U10:
            case Pixel::RGB_U16:
            case Pixel::RGB_F16:
            case Pixel::RGB_F32:
                image.elem[0].descriptor = DESCRIPTOR_RGB;
                break;
            case Pixel::RGBA_U8:
            case Pixel::RGBA_U16:
            case Pixel::RGBA_F16:
            case Pixel::RGBA_F32:
                image.elem[0].descriptor = DESCRIPTOR_RGBA;
                break;
            default: break;
            }

            switch (info.layers[0].pixel)
            {
            case Pixel::RGB_U10:
                image.elem[0].packing = TYPE_A;
                break;
            default: break;
            }

            const int bitDepth = Pixel::bitDepth(info.layers[0].pixel);
            image.elem[0].bitDepth = bitDepth;

            image.elem[0].dataSign = 0;
            image.elem[0].lowData = 0;

            switch (bitDepth)
            {
            case 8:
                image.elem[0].highData = 255;
                break;
            case 10:
                image.elem[0].highData = 1023;
                break;
            case 12:
                image.elem[0].highData = 4095;
                break;
            case 16:
                image.elem[0].highData = 65535;
                break;
            }

            if (Cineon::COLOR_PROFILE_RAW == colorProfile)
            {
                image.elem[0].transfer = TRANSFER_LINEAR;
            }
            else
            {
                image.elem[0].transfer = TRANSFER_FILM_PRINT;
            }

            switch (version)
            {
            case DPX::VERSION_1_0:

                if (Cineon::COLOR_PROFILE_RAW == colorProfile)
                {
                    image.elem[0].colorimetric = COLORIMETRIC_1_0_USER;
                }
                else
                {
                    image.elem[0].colorimetric = COLORIMETRIC_1_0_FILM_PRINT;
                }
                break;
            case DPX::VERSION_2_0:
                if (Cineon::COLOR_PROFILE_RAW == colorProfile)
                {
                    image.elem[0].colorimetric = COLORIMETRIC_2_0_USER;
                }
                else
                {
                    image.elem[0].colorimetric = COLORIMETRIC_2_0_FILM_PRINT;
                }
                break;
            default: break;
            }

            image.elem[0].encoding = 0;
            image.elem[0].dataOffset = 2048;
            image.elem[0].linePadding = 0;
            image.elem[0].elemPadding = 0;

            // File tags.
            const QStringList & tags = Tags::tagLabels();
            const QStringList & dpxTags = DPX::tagLabels();
            QString tmp;
            Core::StringUtil::cString(info.layers[0].fileName, file.name, 100, false);
            tmp = info.tags[tags[Tags::TIME]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, file.time, 24, false);
            }
            tmp = info.tags[tags[Tags::CREATOR]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, file.creator, 100, false);
            }
            tmp = info.tags[tags[Tags::PROJECT]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, file.project, 200, false);
            }
            tmp = info.tags[tags[Tags::COPYRIGHT]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, file.copyright, 200, false);
            }
            file.encryptionKey = 0;

            // Source tags.
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_OFFSET]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (2 == list.count())
                {
                    source.offset[0] = list[0].toInt();
                    source.offset[1] = list[1].toInt();
                }
            }
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_CENTER]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (2 == list.count())
                {
                    source.center[0] = list[0].toFloat();
                    source.center[1] = list[1].toFloat();
                }
            }
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_SIZE]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (2 == list.count())
                {
                    source.size[0] = list[0].toInt();
                    source.size[1] = list[1].toInt();
                }
            }
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_FILE]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.file, 100, false);
            }
            tmp = info.tags[tags[DPX::TAG_SOURCE_TIME]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.time, 24, false);
            }
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_INPUT_DEVICE]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.inputDevice, 32, false);
            }
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_INPUT_SERIAL]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, source.inputSerial, 32, false);
            }
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_BORDER]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (4 == list.count())
                {
                    source.border[0] = list[0].toInt();
                    source.border[1] = list[1].toInt();
                    source.border[2] = list[2].toInt();
                    source.border[3] = list[3].toInt();
                }
            }
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_PIXEL_ASPECT]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (2 == list.count())
                {
                    source.pixelAspect[0] = list[0].toInt();
                    source.pixelAspect[1] = list[1].toInt();
                }
            }
            tmp = info.tags[dpxTags[DPX::TAG_SOURCE_SCAN_SIZE]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (2 == list.count())
                {
                    source.scanSize[0] = list[0].toFloat();
                    source.scanSize[1] = list[1].toFloat();
                }
            }

            // Film tags.
            tmp = info.tags[tags[Tags::KEYCODE]];
            if (tmp.length())
            {
                int id = 0, type = 0, prefix = 0, count = 0, offset = 0;
                Core::Time::stringToKeycode(tmp, id, type, prefix, count, offset);
                Core::StringUtil::cString(
                    QString::number(id), film.id, 2, false);
                Core::StringUtil::cString(
                    QString::number(type), film.type, 2, false);
                Core::StringUtil::cString(
                    QString::number(offset), film.offset, 2, false);
                Core::StringUtil::cString(
                    QString::number(prefix), film.prefix, 6, false);
                Core::StringUtil::cString(
                    QString::number(count), film.count, 4, false);
            }
            tmp = info.tags[dpxTags[DPX::TAG_FILM_FORMAT]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, film.format, 32, false);
            }
            tmp = info.tags[dpxTags[DPX::TAG_FILM_FRAME]];
            if (tmp.length())
            {
                film.frame = tmp.toInt();
            }
            tmp = info.tags[dpxTags[DPX::TAG_FILM_SEQUENCE]];
            if (tmp.length())
            {
                film.sequence = tmp.toInt();
            }
            tmp = info.tags[dpxTags[DPX::TAG_FILM_HOLD]];
            if (tmp.length())
            {
                film.hold = tmp.toInt();
            }
            tmp = info.tags[dpxTags[DPX::TAG_FILM_FRAME_RATE]];
            if (tmp.length())
            {
                film.frameRate = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_FILM_SHUTTER]];
            if (tmp.length())
            {
                film.shutter = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_FILM_FRAME_ID]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, film.frameId, 32, false);
            }
            tmp = info.tags[dpxTags[DPX::TAG_FILM_SLATE]];
            if (tmp.length())
            {
                Core::StringUtil::cString(tmp, film.slate, 100, false);
            }

            // TV tags.
            tmp = info.tags[tags[Tags::TIMECODE]];
            if (tmp.length())
            {
                tv.timecode = Core::Time::stringToTimecode(tmp);
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_INTERLACE]];
            if (tmp.length())
            {
                tv.interlace = tmp.toInt();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_FIELD]];
            if (tmp.length())
            {
                tv.field = tmp.toInt();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_VIDEO_SIGNAL]];
            if (tmp.length())
            {
                tv.videoSignal = tmp.toInt();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_SAMPLE_RATE]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (2 == list.count())
                {
                    tv.sampleRate[0] = list[0].toFloat();
                    tv.sampleRate[1] = list[1].toFloat();
                }
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_FRAME_RATE]];
            if (tmp.length())
            {
                tv.frameRate = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_TIME_OFFSET]];
            if (tmp.length())
            {
                tv.timeOffset = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_GAMMA]];
            if (tmp.length())
            {
                tv.gamma = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_BLACK_LEVEL]];
            if (tmp.length())
            {
                tv.blackLevel = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_BLACK_GAIN]];
            if (tmp.length())
            {
                tv.blackGain = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_BREAK_POINT]];
            if (tmp.length())
            {
                tv.breakpoint = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_WHITE_LEVEL]];
            if (tmp.length())
            {
                tv.whiteLevel = tmp.toFloat();
            }
            tmp = info.tags[dpxTags[DPX::TAG_TV_INTEGRATION_TIMES]];
            if (tmp.length())
                tv.integrationTimes = tmp.toFloat();

            // Write.
            debug();
            Core::Memory::ENDIAN fileEndian = Core::Memory::endian();
            if (DPX::ENDIAN_MSB == endian)
            {
                fileEndian = Core::Memory::MSB;
            }
            else if (DPX::ENDIAN_LSB == endian)
            {
                fileEndian = Core::Memory::LSB;
            }
            if (fileEndian != Core::Memory::endian())
            {
                //DJV_DEBUG_PRINT("endian");
                io.setEndian(true);
                this->endian();
            }
            memcpy(
                &file.magic,
                Core::Memory::MSB == fileEndian ? magic[0] : magic[1],
                4);
            io.set(&file, sizeof(File));
            io.set(&image, sizeof(Image));
            io.set(&source, sizeof(Source));
            io.set(&film, sizeof(Film));
            io.set(&tv, sizeof(Tv));
        }

        void DPXHeader::saveEnd(Core::FileIO & io)
        {
            const quint32 size = static_cast<quint32>(io.pos());
            io.setPos(12);
            io.setU32(size);
        }

        void DPXHeader::zero(char * in, int size)
        {
            memset(in, 0, size);
        }

        bool DPXHeader::isValid(const quint8 * in)
        {
            return *in != 0xff;
        }

        bool DPXHeader::isValid(const quint16 * in)
        {
            return *in != 0xffff;
        }

        namespace
        {
            //! \todo These hard-coded values are meant to catch uninitialized values.
            const qint32 _intMax = 1000000;
            const float  _floatMax = 1000000.f;
            const char   _minChar = 32;
            const char   _maxChar = 126;

        } // namespace

        bool DPXHeader::isValid(const quint32 * in)
        {
            return *in != 0xffffffff && *in < quint32(_intMax);
        }

        bool DPXHeader::isValid(const float * in)
        {
            return
                *((quint32 *)in) != 0xffffffff &&
                *in > -_floatMax &&
                *in < _floatMax;
        }

        bool DPXHeader::isValid(const char * in, int size)
        {
            const char * p = in;
            const char * const end = p + size;
            for (; *p && p < end; ++p)
            {
                if (*p < _minChar || *p > _maxChar)
                {
                    return false;
                }
            }
            return size ? (in[0] != 0) : false;
        }

        QString DPXHeader::toString(const char * in, int size)
        {
            const char * p = in;
            const char * const end = p + size;
            for (; *p && p < end; ++p)
                ;
            return QString(in).mid(0, p - in);
        }

        void DPXHeader::endian()
        {
            Core::Memory::convertEndian(&file.imageOffset, 1, 4);
            Core::Memory::convertEndian(&file.size, 1, 4);
            Core::Memory::convertEndian(&file.dittoKey, 1, 4);
            Core::Memory::convertEndian(&file.headerSize, 1, 4);
            Core::Memory::convertEndian(&file.industryHeaderSize, 1, 4);
            Core::Memory::convertEndian(&file.userHeaderSize, 1, 4);
            Core::Memory::convertEndian(&file.encryptionKey, 1, 4);

            Core::Memory::convertEndian(&image.orient, 1, 2);
            Core::Memory::convertEndian(&image.elemSize, 1, 2);
            Core::Memory::convertEndian(image.size, 2, 4);

            for (uint i = 0; i < 8; ++i)
            {
                Core::Memory::convertEndian(&image.elem[i].dataSign, 1, 4);
                Core::Memory::convertEndian(&image.elem[i].lowData, 1, 4);
                Core::Memory::convertEndian(&image.elem[i].lowQuantity, 1, 4);
                Core::Memory::convertEndian(&image.elem[i].highData, 1, 4);
                Core::Memory::convertEndian(&image.elem[i].highQuantity, 1, 4);
                Core::Memory::convertEndian(&image.elem[i].packing, 1, 2);
                Core::Memory::convertEndian(&image.elem[i].encoding, 1, 2);
                Core::Memory::convertEndian(&image.elem[i].dataOffset, 1, 4);
                Core::Memory::convertEndian(&image.elem[i].linePadding, 1, 4);
                Core::Memory::convertEndian(&image.elem[i].elemPadding, 1, 4);
            }

            Core::Memory::convertEndian(source.offset, 2, 4);
            Core::Memory::convertEndian(source.center, 2, 4);
            Core::Memory::convertEndian(source.size, 2, 4);
            Core::Memory::convertEndian(source.border, 4, 2);
            Core::Memory::convertEndian(source.pixelAspect, 2, 4);
            Core::Memory::convertEndian(source.scanSize, 2, 4);

            Core::Memory::convertEndian(&film.frame, 1, 4);
            Core::Memory::convertEndian(&film.sequence, 1, 4);
            Core::Memory::convertEndian(&film.hold, 1, 4);
            Core::Memory::convertEndian(&film.frameRate, 1, 4);
            Core::Memory::convertEndian(&film.shutter, 1, 4);

            Core::Memory::convertEndian(&tv.timecode, 1, 4);
            Core::Memory::convertEndian(&tv.userBits, 1, 4);
            Core::Memory::convertEndian(tv.sampleRate, 2, 4);
            Core::Memory::convertEndian(&tv.frameRate, 1, 4);
            Core::Memory::convertEndian(&tv.timeOffset, 1, 4);
            Core::Memory::convertEndian(&tv.gamma, 1, 4);
            Core::Memory::convertEndian(&tv.blackLevel, 1, 4);
            Core::Memory::convertEndian(&tv.blackGain, 1, 4);
            Core::Memory::convertEndian(&tv.breakpoint, 1, 4);
            Core::Memory::convertEndian(&tv.whiteLevel, 1, 4);
            Core::Memory::convertEndian(&tv.integrationTimes, 1, 4);
        }

        namespace
        {
        const QString debugFile =
            "File\n"
            "  Image offset = %1\n"
            "  Version = %2\n"
            "  File size = %3\n"
            "  Ditto key = %4\n"
            "  Header size = %5\n"
            "  Industry header size = %6\n"
            "  User header size = %7\n"
            "  Name = %8\n"
            "  Time = %9\n"
            "  Creator = %10\n"
            "  Project = %11\n"
            "  Copyright = %12\n"
            "  Encryption key = %13\n";

        const QString debugImage =
            "Image\n"
            "  Orient = %1\n"
            "  Element size = %2\n"
            "  Size = %3 %4\n";

        const QString debugImageElement =
            "Image Element #%1\n"
            "  Data sign = %2\n"
            "  Low data = %3\n"
            "  Low quantity = %4\n"
            "  High data = %5\n"
            "  High quantity = %6\n"
            "  Descriptor = %7\n"
            "  Transfer = %8\n"
            "  Colorimetric = %9\n"
            "  Bit depth = %10\n"
            "  Packing = %11\n"
            "  Encoding = %12\n"
            "  Data offset = %13\n"
            "  Line padding = %14\n"
            "  Element padding = %15\n"
            "  Description = %16\n";

        const QString debugSource =
            "Source\n"
            "  Offset = %1 %2\n"
            "  Center = %3 %4\n"
            "  Size = %5 %6\n"
            "  File = %7\n"
            "  Time = %8\n"
            "  Input device = %9\n"
            "  Input serial = %10\n"
            "  Border = %11 %12 %13 %14\n"
            "  Pixel aspect = %15 %16\n"
            "  Scan size = %17 %18\n";

        const QString debugFilm =
            "Film\n"
            "  ID = %1\n"
            "  Type = %2\n"
            "  Offset = %3\n"
            "  Prefix = %4\n"
            "  Count = %5\n"
            "  Format = %6\n"
            "  Frame = %7\n"
            "  Sequence = %8\n"
            "  Hold = %9\n"
            "  Frame rate = %10\n"
            "  Shutter = %11\n"
            "  Frame ID = %12\n"
            "  Slate = %13\n";

        const QString debugTv =
            "TV\n"
            "  Timecode = %1\n"
            "  User bits = %2\n"
            "  Interlace = %3\n"
            "  Field = %4\n"
            "  Video signal = %5\n"
            "  Sample rate = %6 %7\n"
            "  Frame rate = %8\n"
            "  Time offset = %9\n"
            "  Gamma = %10\n"
            "  Black level = %11\n"
            "  Black gain = %12\n"
            "  Breakpoint = %13\n"
            "  White level = %14\n"
            "  Integration times = %15\n";

        } // namespace

        QString DPXHeader::debug() const
        {
            QString out;

            out += QString(debugFile).
                arg(debug(file.imageOffset)).
                arg(debug(file.version, 8)).
                arg(debug(file.size)).
                arg(debug(file.dittoKey)).
                arg(debug(file.headerSize)).
                arg(debug(file.industryHeaderSize)).
                arg(debug(file.userHeaderSize)).
                arg(debug(file.name, 100)).
                arg(debug(file.time, 24)).
                arg(debug(file.creator, 100)).
                arg(debug(file.project, 200)).
                arg(debug(file.copyright, 200)).
                arg(debug(file.encryptionKey));

            out += QString(debugImage).
                arg(debug(image.orient)).
                arg(debug(image.elemSize)).
                arg(debug(image.size[0])).
                arg(debug(image.size[1]));

            for (int i = 0; i < image.elemSize; ++i)
                out += QString(debugImageElement).
                    arg(i).
                    arg(debug(image.elem[i].dataSign)).
                    arg(debug(image.elem[i].lowData)).
                    arg(debug(image.elem[i].lowQuantity)).
                    arg(debug(image.elem[i].highData)).
                    arg(debug(image.elem[i].highQuantity)).
                    arg(debug(image.elem[i].descriptor)).
                    arg(debug(image.elem[i].transfer)).
                    arg(debug(image.elem[i].colorimetric)).
                    arg(debug(image.elem[i].bitDepth)).
                    arg(debug(image.elem[i].packing)).
                    arg(debug(image.elem[i].encoding)).
                    arg(debug(image.elem[i].dataOffset)).
                    arg(debug(image.elem[i].linePadding)).
                    arg(debug(image.elem[i].elemPadding)).
                    arg(debug(image.elem[i].description, 32));

            out += QString(debugSource).
                arg(debug(source.offset[0])).
                arg(debug(source.offset[1])).
                arg(debug(source.center[0])).
                arg(debug(source.center[1])).
                arg(debug(source.size[0])).
                arg(debug(source.size[1])).
                arg(debug(source.file, 100)).
                arg(debug(source.time, 24)).
                arg(debug(source.inputDevice, 32)).
                arg(debug(source.inputSerial, 32)).
                arg(debug(source.border[0])).
                arg(debug(source.border[1])).
                arg(debug(source.border[2])).
                arg(debug(source.border[3])).
                arg(debug(source.pixelAspect[0])).
                arg(debug(source.pixelAspect[1])).
                arg(debug(source.scanSize[0])).
                arg(debug(source.scanSize[1]));

            out += QString(debugFilm).
                arg(debug(film.id, 2)).
                arg(debug(film.type, 2)).
                arg(debug(film.offset, 2)).
                arg(debug(film.prefix, 6)).
                arg(debug(film.count, 4)).
                arg(debug(film.format, 32)).
                arg(debug(film.frame)).
                arg(debug(film.sequence)).
                arg(debug(film.hold)).
                arg(debug(film.frameRate)).
                arg(debug(film.shutter)).
                arg(debug(film.frameId, 32)).
                arg(debug(film.slate, 100));

            out += QString(debugTv).
                arg(debug(tv.timecode)).
                arg(debug(tv.userBits)).
                arg(debug(tv.interlace)).
                arg(debug(tv.field)).
                arg(debug(tv.videoSignal)).
                arg(debug(tv.sampleRate[0])).
                arg(debug(tv.sampleRate[1])).
                arg(debug(tv.frameRate)).
                arg(debug(tv.timeOffset)).
                arg(debug(tv.gamma)).
                arg(debug(tv.blackLevel)).
                arg(debug(tv.blackGain)).
                arg(debug(tv.breakpoint)).
                arg(debug(tv.whiteLevel)).
                arg(debug(tv.integrationTimes));

            return out;
        }

        QString DPXHeader::debug(quint8 in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString DPXHeader::debug(quint16 in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString DPXHeader::debug(quint32 in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString DPXHeader::debug(float in)
        {
            return isValid(&in) ? QString::number(in) : "[]";
        }

        QString DPXHeader::debug(const char * in, int size)
        {
            return isValid(in, size) ? toString(in, size) : "[]";
        }

    } // namespace AV
} // namespace djv
