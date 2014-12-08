//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvViewFilePrefs.h

#ifndef DJV_VIEW_FILE_PREFS_H
#define DJV_VIEW_FILE_PREFS_H

#include <djvViewAbstractPrefs.h>

#include <djvFileInfo.h>
#include <djvPixelData.h>

#include <QStringList>

//! \addtogroup djvViewFile
//@{

//------------------------------------------------------------------------------
//! \class djvViewFilePrefs
//!
//! This class provides the file group preferences.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewFilePrefs : public djvViewAbstractPrefs
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewFilePrefs(QObject * parent = 0);

    //! Destructor.

    virtual ~djvViewFilePrefs();

    //! Add a recent file.

    void addRecent(const djvFileInfo &);

    //! Get the recent files.

    const djvFileInfoList & recentFiles() const;

    //! Get the default for whether sequences are automatically opened.
    
    static bool autoSequenceDefault();

    //! Get whether sequences are automatically opened.

    bool hasAutoSequence() const;

    //! Get the default for whether to combine command line arguments.
    
    static bool combineCommandLineDefault();

    //! Get whether to combine command line arguments.

    bool hasCombineCommandLine() const;

    //! Get the default proxy scale.
    
    static djvPixelDataInfo::PROXY proxyDefault();

    //! Get the proxy scale.

    djvPixelDataInfo::PROXY proxy() const;

    //! Get the default for whether images are converted to 8-bits.
    
    static bool u8ConversionDefault();
    
    //! Get whether images are converted to 8-bits.
    
    bool hasU8Conversion() const;

    //! Get the default for whether the cache is enabled.
    
    static bool cacheEnabledDefault();

    //! Get whether the cache is enabled.

    bool isCacheEnabled() const;

    //! Get the default cache size in gigabytes.
    
    static double cacheSizeDefault();

    //! Get the cache size in gigabytes.

    double cacheSize() const;

    //! Get the default for whether the cache is displayed in the timeline.
    
    static bool cacheDisplayDefault();

    //! Get whether the cache is displayed in the timeline.

    bool hasCacheDisplay() const;

    //! Get the global preferences.

    static djvViewFilePrefs * global();

public Q_SLOTS:

    //! Set whether sequences are automatically opened.

    void setAutoSequence(bool);

    //! Set whether to combine command line arguments.

    void setCombineCommandLine(bool);
    
    //! Set the proxy scale.

    void setProxy(djvPixelDataInfo::PROXY);

    //! Set whether images are converted to 8-bits.
    
    void setU8Conversion(bool);
    
    //! Set whether the cache is enabled.

    void setCacheEnabled(bool);

    //! Set the cache size in gigabytes.

    void setCacheSize(double);
    
    //! Set whether the cache is displayed in the timeline.

    void setCacheDisplay(bool);

Q_SIGNALS:

    //! This signal is emitted when the recent files are changed.

    void recentChanged(const djvFileInfoList &);
    
    //! This signal is emitted when automatic sequences is changed.

    void autoSequenceChanged(bool);
    
    //! This signal is emitted when the proxy scale is changed.

    void proxyChanged(djvPixelDataInfo::PROXY);
    
    //! This signal is emitted when 8-bit conversion is changed.

    void u8ConversionChanged(bool);
    
    //! This signal is emitted when the cache is enabled or disabled.

    void cacheEnabledChanged(bool);
    
    //! This signal is emitted when the cache size is changed.
    
    void cacheSizeChanged(double);
    
    //! This signal is emitted when the cache display is changed.

    void cacheDisplayChanged(bool);

private:

    djvFileInfoList         _recent;
    bool                    _autoSequence;
    bool                    _combineCommandLine;
    djvPixelDataInfo::PROXY _proxy;
    bool                    _u8Conversion;
    bool                    _cacheEnabled;
    double                  _cacheSize;
    bool                    _cacheDisplay;
};

//@} // djvViewFile

#endif // DJV_VIEW_FILE_PREFS_H

