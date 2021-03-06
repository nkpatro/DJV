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

#include <djvAV/Cineon.h>

namespace djv
{
    namespace AV
    {
        //! This struct provides DPX utilities.
        struct DPX
        {
            //! Plugin name.
            static const QString staticName;

            //! This enumeration provides the file format version.
            enum VERSION
            {
                VERSION_1_0,
                VERSION_2_0,

                VERSION_COUNT
            };

            //! Get the file format version labels.
            static const QStringList & versionLabels();

            //! This enumeration provides the file types.
            enum TYPE
            {
                TYPE_AUTO,
                TYPE_U10,

                TYPE_COUNT
            };

            //! Get the file type labels.
            static const QStringList & typeLabels();

            //! This enumeration provides the file endians.
            enum ENDIAN
            {
                ENDIAN_AUTO,
                ENDIAN_MSB,
                ENDIAN_LSB,

                ENDIAN_COUNT
            };

            //! Get the file endian labels.
            static const QStringList & endianLabels();

            //! This enumeration provides the DPX tags.
            enum TAG
            {
                TAG_SOURCE_OFFSET,
                TAG_SOURCE_CENTER,
                TAG_SOURCE_SIZE,
                TAG_SOURCE_FILE,
                TAG_SOURCE_TIME,
                TAG_SOURCE_INPUT_DEVICE,
                TAG_SOURCE_INPUT_SERIAL,
                TAG_SOURCE_BORDER,
                TAG_SOURCE_PIXEL_ASPECT,
                TAG_SOURCE_SCAN_SIZE,
                TAG_FILM_FORMAT,
                TAG_FILM_FRAME,
                TAG_FILM_SEQUENCE,
                TAG_FILM_HOLD,
                TAG_FILM_FRAME_RATE,
                TAG_FILM_SHUTTER,
                TAG_FILM_FRAME_ID,
                TAG_FILM_SLATE,
                TAG_TV_INTERLACE,
                TAG_TV_FIELD,
                TAG_TV_VIDEO_SIGNAL,
                TAG_TV_SAMPLE_RATE,
                TAG_TV_FRAME_RATE,
                TAG_TV_TIME_OFFSET,
                TAG_TV_GAMMA,
                TAG_TV_BLACK_LEVEL,
                TAG_TV_BLACK_GAIN,
                TAG_TV_BREAK_POINT,
                TAG_TV_WHITE_LEVEL,
                TAG_TV_INTEGRATION_TIMES,

                TAG_COUNT
            };

            //! Get the DPX tag labels.
            static const QStringList & tagLabels();

            //! This enumeration provides the options.
            enum OPTIONS
            {
                INPUT_COLOR_PROFILE_OPTION,
                INPUT_FILM_PRINT_OPTION,
                OUTPUT_COLOR_PROFILE_OPTION,
                OUTPUT_FILM_PRINT_OPTION,
                VERSION_OPTION,
                TYPE_OPTION,
                ENDIAN_OPTION,

                OPTIONS_COUNT
            };

            //! Get the option labels.
            static const QStringList & optionsLabels();

            //! This struct provides options.
            struct Options
            {
                Cineon::COLOR_PROFILE     inputColorProfile  = Cineon::COLOR_PROFILE_AUTO;
                Cineon::FilmPrintToLinear inputFilmPrint;
                Cineon::COLOR_PROFILE     outputColorProfile = Cineon::COLOR_PROFILE_FILM_PRINT;
                Cineon::LinearToFilmPrint outputFilmPrint;
                DPX::VERSION              version            = DPX::VERSION_2_0;
                DPX::TYPE                 type               = DPX::TYPE_U10;
                DPX::ENDIAN               endian             = DPX::ENDIAN_MSB;
            };
        };

    } // namespace AV

    DJV_STRING_OPERATOR(AV::DPX::VERSION);
    DJV_STRING_OPERATOR(AV::DPX::TYPE);
    DJV_STRING_OPERATOR(AV::DPX::ENDIAN);

} // namespace djv
