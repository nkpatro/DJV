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

#include <djvViewLib/ControlsWindow.h>

#include <djvViewLib/AnnotateGroup.h>
#include <djvViewLib/FileGroup.h>
#include <djvViewLib/HelpGroup.h>
#include <djvViewLib/ImageGroup.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/StatusBar.h>
#include <djvViewLib/ToolGroup.h>
#include <djvViewLib/ViewGroup.h>
#include <djvViewLib/WindowGroup.h>

#include <QCloseEvent>
#include <QMenuBar>
#include <QToolBar>

namespace djv
{
    namespace ViewLib
    {
        struct ControlsWindow::Private
        {
            QMap<Enum::UI_COMPONENT, QPointer<QToolBar> > toolBars;
            QPointer<QToolBar> playbackControls;
            QPointer<Session> session;
        };

        ControlsWindow::ControlsWindow(
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            QMainWindow(parent),
            _p(new Private)
        {
            _p->session = session;

            setWindowFlags(Qt::Dialog);

            menuBar()->setNativeMenuBar(false);
            menuBar()->addMenu(session->fileGroup()->createMenu());
            menuBar()->addMenu(session->windowGroup()->createMenu());
            menuBar()->addMenu(session->viewGroup()->createMenu());
            menuBar()->addMenu(session->imageGroup()->createMenu());
            menuBar()->addMenu(session->playbackGroup()->createMenu());
            menuBar()->addMenu(session->toolGroup()->createMenu());
            menuBar()->addMenu(session->annotateGroup()->createMenu());
            menuBar()->addMenu(session->helpGroup()->createMenu());

            _p->toolBars[Enum::UI_FILE_TOOL_BAR] = session->fileGroup()->createToolBar();
            _p->toolBars[Enum::UI_WINDOW_TOOL_BAR] = session->windowGroup()->createToolBar();
            _p->toolBars[Enum::UI_VIEW_TOOL_BAR] = session->viewGroup()->createToolBar();
            _p->toolBars[Enum::UI_IMAGE_TOOL_BAR] = session->imageGroup()->createToolBar();
            _p->toolBars[Enum::UI_TOOLS_TOOL_BAR] = session->toolGroup()->createToolBar();
            _p->toolBars[Enum::UI_ANNOTATE_TOOL_BAR] = session->annotateGroup()->createToolBar();
            Q_FOREACH(auto toolBar, _p->toolBars)
            {
                addToolBar(toolBar);
            }
            _p->playbackControls = session->playbackGroup()->createToolBar();
            addToolBar(Qt::ToolBarArea::BottomToolBarArea, _p->playbackControls);

            setStatusBar(new StatusBar(session, context));

            widgetUpdate();

            connect(
                session->windowGroup(),
                SIGNAL(uiComponentVisibleChanged(const QMap<djv::ViewLib::Enum::UI_COMPONENT, bool> &)),
                SLOT(widgetUpdate()));
        }

        ControlsWindow::~ControlsWindow()
        {}

        QMenu * ControlsWindow::createPopupMenu()
        {
            auto out = new QMenu;
            out->addMenu(_p->session->fileGroup()->createMenu());
            out->addMenu(_p->session->windowGroup()->createMenu());
            out->addMenu(_p->session->viewGroup()->createMenu());
            out->addMenu(_p->session->imageGroup()->createMenu());
            out->addMenu(_p->session->playbackGroup()->createMenu());
            out->addMenu(_p->session->toolGroup()->createMenu());
            out->addMenu(_p->session->annotateGroup()->createMenu());
            out->addMenu(_p->session->helpGroup()->createMenu());
            return out;
        }

        void ControlsWindow::closeEvent(QCloseEvent * event)
        {
            event->ignore();
            Q_EMIT closed();
        }

        void ControlsWindow::keyPressEvent(QKeyEvent * event)
        {
            QMainWindow::keyPressEvent(event);
            switch (event->key())
            {
            case Qt::Key_Escape:
                if (_p->session->windowGroup()->hasFullScreen())
                {
                    _p->session->windowGroup()->setFullScreen(false);
                    _p->session->viewWidget()->viewFit();
                }
                break;
            }
        }

        void ControlsWindow::widgetUpdate()
        {
            const auto & visible = _p->session->windowGroup()->uiComponentVisible();
            Q_FOREACH(auto key, _p->toolBars.keys())
            {
                _p->toolBars[key]->setVisible(visible[key]);
            }
            _p->playbackControls->setVisible(visible[Enum::UI_PLAYBACK_CONTROLS]);
            statusBar()->setVisible(visible[Enum::UI_STATUS_BAR]);
        }

    } // namespace ViewLib
} // namespace djv
