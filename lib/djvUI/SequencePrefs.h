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

#include <djvUI/Core.h>

#include <djvCore/Sequence.h>
#include <djvCore/Util.h>

#include <QObject>

#include <memory>

namespace djv
{
    namespace UI
    {
        //! This class provides file sequence preferences.
        class SequencePrefs : public QObject
        {
            Q_OBJECT

            //! This property holds the file sequence formatting.
            Q_PROPERTY(
                Core::Sequence::FORMAT format
                READ                   format
                WRITE                  setFormat
                NOTIFY                 formatChanged);

            //! This property holds whether auto file sequencing is enabled.
            Q_PROPERTY(
                bool   autoEnabled
                READ   isAutoEnabled
                WRITE  setAutoEnabled
                NOTIFY autoEnabledChanged);

            //! This property holds the maximum file sequence size.
            Q_PROPERTY(
                qint64 maxSize
                READ   maxSize
                WRITE  setMaxSize
                NOTIFY maxSizeChanged);

            //! This property holds whether negative numbers are enabled.
            Q_PROPERTY(
                bool   negativeEnabled
                READ   isNegativeEnabled
                WRITE  setNegativeEnabled
                NOTIFY negativeEnabledChanged);

        public:
            explicit SequencePrefs(QObject * parent = nullptr);
            ~SequencePrefs();

            //! Get the file sequence formatting.
            Core::Sequence::FORMAT format() const;

            //! Get whether auto file sequencing is enabled.
            bool isAutoEnabled() const;

            //! Get the maximum file sequence size.
            qint64 maxSize() const;

            //! Get whether negative numbers are enabled.
            bool isNegativeEnabled() const;

        public Q_SLOTS:
            //! Set the file sequence formatting.
            void setFormat(djv::Core::Sequence::FORMAT);

            //! Set whether auto file sequencing is enabled.
            void setAutoEnabled(bool);

            //! Set the maximum file sequence size.
            void setMaxSize(qint64);

            //! Set whether negative numbers are enabled.
            void setNegativeEnabled(bool);
            
        Q_SIGNALS:
            //! This signal is emitted when the file sequence formatting is changed.
            void formatChanged(djv::Core::Sequence::FORMAT);

            //! This signal is emitted when auto file sequencing is changed.
            void autoEnabledChanged(bool);

            //! This signal is emitted when the maximum file sequence size is changed.
            void maxSizeChanged(qint64);

            //! This signal is emitted when negative numbers are changed.
            void negativeEnabledChanged(bool);

            //! This signal is emitted when a preference is changed.
            void prefChanged();
            
        private:
            DJV_PRIVATE_COPY(SequencePrefs);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
