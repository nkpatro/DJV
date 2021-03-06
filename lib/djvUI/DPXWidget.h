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

#include <djvUI/IOWidget.h>

#include <djvAV/DPX.h>

class QCheckBox;
class QComboBox;
class QFormLayout;
class QVBoxLayout;

namespace djv
{
    namespace UI
    {
        class FloatEditSlider;
        class IntEditSlider;

        //! This class provides a DPX widget.
        class DPXWidget : public IOWidget
        {
            Q_OBJECT

        public:
            DPXWidget(AV::IOPlugin *, const QPointer<UIContext> &);

            void resetPreferences() override;

        private Q_SLOTS:
            void pluginCallback(const QString &);
            void inputColorProfileCallback(int);
            void inputBlackPointCallback(int);
            void inputWhitePointCallback(int);
            void inputGammaCallback(float);
            void inputSoftClipCallback(int);
            void outputColorProfileCallback(int);
            void outputBlackPointCallback(int);
            void outputWhitePointCallback(int);
            void outputGammaCallback(float);
            void versionCallback(int);
            void typeCallback(int);
            void endianCallback(int);
            
            void pluginUpdate();
            void widgetUpdate();

        private:
            AV::DPX::Options _options;
            QFormLayout * _inputColorProfileLayout = nullptr;
            QComboBox * _inputColorProfileWidget = nullptr;
            IntEditSlider * _inputBlackPointWidget = nullptr;
            IntEditSlider * _inputWhitePointWidget = nullptr;
            FloatEditSlider * _inputGammaWidget = nullptr;
            IntEditSlider * _inputSoftClipWidget = nullptr;
            QFormLayout * _outputColorProfileLayout = nullptr;
            QComboBox * _outputColorProfileWidget = nullptr;
            IntEditSlider * _outputBlackPointWidget = nullptr;
            IntEditSlider * _outputWhitePointWidget = nullptr;
            FloatEditSlider * _outputGammaWidget = nullptr;
            QComboBox * _versionWidget = nullptr;
            QComboBox * _typeWidget = nullptr;
            QComboBox * _endianWidget = nullptr;
            QVBoxLayout * _layout = nullptr;
        };

        //! This class provides a DPX widget plugin.
        class DPXWidgetPlugin : public IOWidgetPlugin
        {
        public:
            DPXWidgetPlugin(const QPointer<Core::CoreContext> &);

            IOWidget * createWidget(AV::IOPlugin *) const override;
            QString pluginName() const override;
        };

    } // namespace UI
} // namespace djv
