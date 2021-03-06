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

#include <djvViewLib/AnnotateExport.h>

#include <djvViewLib/AnnotateData.h>
#include <djvViewLib/AnnotateExportDialog.h>
#include <djvViewLib/Util.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/NoticeDialog.h>
#include <djvUI/ProgressDialog.h>

#include <djvAV/Image.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/Error.h>

#include <QApplication>
#include <QDir>
#include <QImage>
#include <QPainter>

namespace djv
{
    namespace ViewLib
    {
        struct AnnotateExport::Private
        {
            Private(const QPointer<ViewContext> & context) :
                context(context)
            {}

            AnnotateExportInfo info;
            std::unique_ptr<AV::Load> load;
            AV::PixelDataInfo pixelInfo;
            Core::Sequence inputSequence;
            std::unique_ptr<AV::Save> save;
            std::unique_ptr<AV::OpenGLImage> openGLImage;
            QPointer<UI::ProgressDialog> progressDialog;
            QPointer<QProcess> scriptProcess;
            QPointer<AnnotateExportDialog> scriptDialog;
            QPointer<ViewContext> context;
        };

        AnnotateExport::AnnotateExport(const QPointer<ViewContext> & context, QObject * parent) :
            QObject(parent),
            _p(new Private(context))
        {
            _p->progressDialog = new UI::ProgressDialog;
            _p->scriptProcess = new QProcess;
            _p->scriptDialog = new AnnotateExportDialog(_p->scriptProcess);

            connect(
                _p->progressDialog,
                SIGNAL(progressSignal(int)),
                SLOT(progressCallback(int)));
            connect(
                _p->progressDialog,
                SIGNAL(finishedSignal()),
                SLOT(finishedCallback()));
            connect(
                _p->progressDialog,
                SIGNAL(rejected()),
                SLOT(cancel()));
            connect(
                _p->scriptDialog,
                SIGNAL(rejected()),
                SLOT(cancel()));
        }

        AnnotateExport::~AnnotateExport()
        {
            //DJV_DEBUG("AnnotateExport::~AnnotateExport");
            delete _p->scriptDialog;
            delete _p->progressDialog;
            if (_p->openGLImage)
            {
                _p->context->makeGLContextCurrent();
                _p->openGLImage.reset();
            }
        }

        void AnnotateExport::start(const AnnotateExportInfo & info)
        {
            //DJV_DEBUG("AnnotateExport::start");
            //DJV_DEBUG_PRINT("output = " << info.outputFile);
            //DJV_DEBUG_PRINT("sequence = " << info.sequence);

            cancel();

            _p->info = info;

            // Open input.
            try
            {
                AV::IOInfo ioInfo;
                _p->load = _p->context->ioFactory()->load(_p->info.inputFile, ioInfo);

                _p->pixelInfo = AV::PixelDataInfo();
                if (ioInfo.layers.size() > 0)
                {
                    _p->pixelInfo.pixel = AV::Pixel::RGBA_U8;
                    _p->pixelInfo.size = ioInfo.layers[0].size;
                }
                _p->inputSequence = ioInfo.sequence;
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_OPEN_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.inputFile)));
                _p->context->printError(error);
                return;
            }

            // Open output.
            AV::IOInfo saveInfo(_p->pixelInfo);
            for (size_t frame = 0; frame < _p->info.primitives.size(); ++frame)
            {
                saveInfo.sequence.frames.push_back(static_cast<qint64>(frame));
            }
            try
            {
                _p->save = _p->context->ioFactory()->save(_p->info.outputFile, saveInfo);
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_OPEN_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.outputFile)));
                _p->context->printError(error);
                return;
            }

            // Start...
            _p->progressDialog->setLabel(qApp->translate("djv::ViewLib::AnnotateExport", "Exporting \"%1\":").
                arg(QDir::toNativeSeparators(_p->info.outputFile)));
            _p->progressDialog->start(static_cast<int>(_p->info.primitives.size()));
            _p->progressDialog->show();
            _p->progressDialog->activateWindow();
        }

        void AnnotateExport::cancel()
        {
            //DJV_DEBUG("AnnotateExport::cancel");

            if (_p->progressDialog->isVisible())
            {
                _p->progressDialog->reject();
            }
            _p->load.reset();
            if (_p->save)
            {
                try
                {
                    //DJV_DEBUG_PRINT("close");
                    _p->save->close();
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                        arg(QDir::toNativeSeparators(_p->info.outputFile)));
                    _p->context->printError(error);
                }
                _p->save.reset();
            }

            if (_p->scriptDialog->isVisible())
            {
                _p->scriptDialog->reject();
            }
            _p->scriptProcess->close();
        }

        void AnnotateExport::progressCallback(int in)
        {
            //DJV_DEBUG("AnnotateExport::callback");
            //DJV_DEBUG_PRINT("in = " << in);

            _p->context->makeGLContextCurrent();
            if (!_p->openGLImage)
            {
                _p->openGLImage.reset(new AV::OpenGLImage);
            }

            // Load the frame.
            AV::Image image;
            try
            {
                const qint64 frame = _p->info.primitives[in].first;
                //DJV_DEBUG_PRINT("frame = " << frame);
                //DJV_DEBUG_PRINT("sequence = " << sequence);
                _p->load->read(
                    image,
                    AV::ImageIOInfo(
                        frame < _p->inputSequence.frames.count() ?
                            _p->inputSequence.frames[frame] :
                            -1,
                        _p->info.layer,
                        _p->info.proxy));
                //DJV_DEBUG_PRINT("image = " << image);
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_READ_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.inputFile)));
                _p->context->printError(error);
                cancel();
                return;
            }

            // Process the frame.
            AV::Image* p = &image;
            AV::Image tmp;
            AV::OpenGLImageOptions options(_p->info.options);
            options.colorProfile = image.colorProfile;
            if (p->info() != _p->pixelInfo || options != AV::OpenGLImageOptions())
            {
                tmp.set(_p->pixelInfo);
                tmp.tags = image.tags;
                try
                {
                    _p->openGLImage->copy(image, tmp, options);
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                        arg(QDir::toNativeSeparators(_p->info.outputFile)));
                    _p->context->printError(error);
                    cancel();
                    return;
                }
                p = &tmp;
            }

            // Draw the annotations.
            QImage qImage(p->data(), _p->pixelInfo.size.x, _p->pixelInfo.size.y, QImage::Format_RGBA8888);
            QPainter painter(&qImage);
            painter.setRenderHints(QPainter::TextAntialiasing);

            QMatrix m;
            m.translate(0, _p->pixelInfo.size.y);
            m.scale(1, -1);
            painter.setMatrix(m);

            Annotate::DrawData drawData;
            drawData.viewSize = _p->pixelInfo.size;
            drawData.selected = true;
            for (const auto& i : _p->info.primitives[in].second)
            {
                i->draw(painter, drawData);
            }

            // Save the frame.
            try
            {
                _p->save->write(*p, in);
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.outputFile)));
                _p->context->printError(error);
                cancel();
                return;
            }
        }

        void AnnotateExport::finishedCallback()
        {
            //DJV_DEBUG("AnnotateExport::finishedCallback");
            _p->load.reset();
            try
            {
                _p->save->close();
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.outputFile)));
                _p->context->printError(error);
            }
            _p->save.reset();

            if (!_p->info.scriptFile.isEmpty())
            {
                //DJV_DEBUG_PRINT("script = " << _p->info.scriptFile);
                //DJV_DEBUG_PRINT("interpreter = " << _p->info.scriptInterpreter);
                QStringList args;
                if (!_p->info.scriptInterpreter.isEmpty())
                {
                    args.append(_p->info.scriptFile);
                }
                args.append(_p->info.jsonFile);
                args.append(_p->info.outputFile);
                args.append(_p->info.scriptOptions.split(" "));
                //DJV_DEBUG_PRINT("args = " << args);
                _p->scriptProcess->start(!_p->info.scriptInterpreter.isEmpty() ? _p->info.scriptInterpreter : _p->info.scriptFile, args);
                _p->scriptDialog->show();
                _p->scriptDialog->activateWindow();
            }
        }

    } // namespace ViewLib
} // namespace djv
