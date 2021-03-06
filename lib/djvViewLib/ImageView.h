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

#include <djvViewLib/Enum.h>

#include <djvUI/ImageView.h>

#include <memory>

namespace djv
{
    namespace Core
    {
        class FileInfo;
    
    } // namespace Core

    namespace ViewLib
    {
        class ViewContext;
        
        struct HudInfo;

        namespace Annotate
        {
            class Data;

        } // namespace Annotate

        //! This class provides an image view widget.
        class ImageView : public UI::ImageView
        {
            Q_OBJECT

        public:
            explicit ImageView(const QPointer<ViewContext> &, QWidget * parent = nullptr);
            ~ImageView() override;

            //! Get whether the mouse is inside the view.
            bool isMouseInside() const;

            //! Get the mouse position.
            const glm::ivec2 & mousePos() const;

            QSize sizeHint() const override;
            QSize minimumSizeHint() const override;

        public Q_SLOTS:
            //! Set the zoom using the mouse pointer for focus.
            void setZoomFocus(float);

            //! Set the grid.
            void setGrid(djv::ViewLib::Enum::GRID);

            //! Set the grid color.
            void setGridColor(const djv::AV::Color &);

            //! Set whether the HUD is enabled.
            void setHudEnabled(bool);

            //! Set the HUD information.
            void setHudInfo(const djv::ViewLib::HudInfo &);

            //! Set the HUD color.
            void setHudColor(const djv::AV::Color &);

            //! Set the HUD background.
            void setHudBackground(djv::ViewLib::Enum::HUD_BACKGROUND);

            //! Set the HUD background color.
            void setHudBackgroundColor(const djv::AV::Color &);

            //! Set the annotations.
            void setAnnotations(const QList<djv::ViewLib::Annotate::Data *> &);

            //! Set the current annotation.
            void setCurrentAnnotation(djv::ViewLib::Annotate::Data *);

            //! Set whether the annotations are visible.
            void setAnnotationsVisible(bool);

        Q_SIGNALS:
            //! This signal is emitted when a pick is started.
            void pickPressed(const glm::ivec2 &);

            //! This signal is emitted when a pick is finished.
            void pickReleased(const glm::ivec2 &);

            //! This signal is emitted during a pick drag.
            void pickMoved(const glm::ivec2 &);

            //! This signal is emitted when the mouse wheel action is changed.
            void mouseWheelActionChanged(djv::ViewLib::Enum::MOUSE_WHEEL_ACTION);

            //! This signal is emitted when the mouse wheel value is changed.
            void mouseWheelValueChanged(int);

            //! This signal is emitted when a context menu is requested.
            void contextMenuRequested(const QPoint &);

            //! This signal is emitted when a file is dropped on the view.
            void fileDropped(const djv::Core::FileInfo &);

        protected:
            void timerEvent(QTimerEvent *) override;
            void enterEvent(QEvent *) override;
            void leaveEvent(QEvent *) override;
            void resizeEvent(QResizeEvent *) override;
            void mousePressEvent(QMouseEvent *) override;
            void mouseReleaseEvent(QMouseEvent *) override;
            void mouseMoveEvent(QMouseEvent *) override;
            void wheelEvent(QWheelEvent *) override;
            void dragEnterEvent(QDragEnterEvent *) override;
            void dropEvent(QDropEvent *) override;
            void initializeGL() override;
            void paintGL() override;

        private Q_SLOTS:
            void hudInfoCallback(const QMap<djv::ViewLib::Enum::HUD, bool> &);

        private:
            void drawGrid(QPainter &);
            void drawHud(QPainter &);
            void drawHudItem(
                QPainter &,
                const QString &,
                const Core::Box2i &);
            void drawAnnotations(QPainter &);

            DJV_PRIVATE_COPY(ImageView);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
