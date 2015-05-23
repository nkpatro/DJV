//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvFFmpegSave.h

#ifndef DJV_FFMPEG_SAVE_H
#define DJV_FFMPEG_SAVE_H

#include <djvFFmpegPlugin.h>

#include <djvFileInfo.h>
#include <djvImage.h>

//! \addtogroup djvFFmpegPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvFFmpegSave
//!
//! This class provides a FFmpeg saver.
//------------------------------------------------------------------------------

class djvFFmpegSave : public djvImageSave
{
public:

    //! Constructor.

    djvFFmpegSave(const djvFFmpegPlugin::Options &);

    //! Destructor.

    virtual ~djvFFmpegSave();

    virtual void open(const djvFileInfo &, const djvImageIoInfo &)
        throw (djvError);

    virtual void write(const djvImage &, const djvImageIoFrameInfo &)
        throw (djvError);

    virtual void close() throw (djvError);

private:

    djvFFmpegPlugin::Options _options;
    djvPixelDataInfo         _info;
    djvImage                 _image;
    
    AVIOContext *            _avIoContext;
    AVFormatContext *        _avFormatContext;
    AVStream *               _avStream;
    AVFrame *                _avFrame;
    uint8_t *                _avFrameBuf;
    AVFrame *                _avFrameRgb;
    SwsContext *             _swsContext;
};

//@} // djvFFmpegPlugin

#endif // DJV_FFMPEG_SAVE_H

