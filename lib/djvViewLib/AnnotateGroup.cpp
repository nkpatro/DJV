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

#include <djvViewLib/AnnotateGroup.h>

#include <djvViewLib/AnnotateActions.h>
#include <djvViewLib/AnnotateData.h>
#include <djvViewLib/AnnotateColorDialog.h>
#include <djvViewLib/AnnotateMenu.h>
#include <djvViewLib/AnnotatePrefs.h>
#include <djvViewLib/AnnotateTool.h>
#include <djvViewLib/AnnotateToolBar.h>
#include <djvViewLib/FileGroup.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/QuestionDialog.h>
#include <djvUI/StylePrefs.h>

#include <djvCore/FileIO.h>
#include <djvCore/PicoJSON.h>

#include <QApplication>
#include <QDockWidget>
#include <QStyle>

#undef DELETE

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            const QString jsonExtension = ".annotations.json";
            const QString exportSuffix = "annotation.";

            QString cleanBaseName(QString value)
            {
                while (value.count() > 0 && ('.' == value[value.count() - 1] || ' ' == value[value.count() - 1]))
                {
                    value.chop(1);
                }
                return value;
            }

        } // namespace

        struct AnnotateGroup::Private
        {
            Private(const QPointer<ViewContext> & context) :
                primitive(context->annotatePrefs()->primitive()),
                color(context->annotatePrefs()->color()),
                lineWidth(context->annotatePrefs()->lineWidth())
            {}

            FileInfo                 fileInfo;
            FileInfo                 jsonFileInfo;
            FileInfo                 exportFileInfo;
            Sequence                 sequence;
            Speed                    speed;
            QList<Annotate::Data *>  annotations;
            QList<Annotate::Data *>  frameAnnotations;
            QPointer<Annotate::Data> currentAnnotation;
            Enum::ANNOTATE_PRIMITIVE primitive;
            AV::Color                color;
            size_t                   lineWidth;
            QString                  summary;

            QPointer<AnnotateActions> actions;
            QPointer<AnnotateTool>    annotateTool;
            QPointer<QDockWidget>     annotateToolDockWidget;
        };

        AnnotateGroup::AnnotateGroup(
            const QPointer<AnnotateGroup>& copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            AbstractGroup(session, context),
            _p(new Private(context))
        {
            if (copy)
            {
            }

            _p->actions = new AnnotateActions(context, this);

            _p->annotateTool = new AnnotateTool(_p->actions, this, session, context);
            _p->annotateToolDockWidget = new QDockWidget(qApp->translate("djv::ViewLib::AnnotateGroup", "Annotate"));
            _p->annotateToolDockWidget->setWidget(_p->annotateTool);

            update();

            connect(
                _p->actions->action(AnnotateActions::SHOW),
                &QAction::toggled,
                [this](bool value)
            {
                update();
                Q_EMIT annotationsVisibleChanged(value);
            });
            connect(
                _p->actions->action(AnnotateActions::COLOR),
                &QAction::triggered,
                [this, context]
            {
                auto dialog = QSharedPointer<AnnotateColorDialog>(new AnnotateColorDialog(context));
                dialog->setColor(_p->color);
                connect(
                    dialog.data(),
                    &AnnotateColorDialog::colorChanged,
                    [this, context](const AV::Color & value)
                {
                    _p->color = value;
                    context->annotatePrefs()->setColor(_p->color);
                    update();
                });
                dialog->exec();
            });
            connect(
                _p->actions->action(AnnotateActions::LINE_WIDTH_INC),
                &QAction::triggered,
                [this, context]
            {
                _p->lineWidth = static_cast<size_t>(Math::clamp(static_cast<int>(_p->lineWidth) + 10, 1, 100));
                context->annotatePrefs()->setLineWidth(_p->lineWidth);
            });
            connect(
                _p->actions->action(AnnotateActions::LINE_WIDTH_DEC),
                &QAction::triggered,
                [this, context]
            {
                _p->lineWidth = static_cast<size_t>(Math::clamp(static_cast<int>(_p->lineWidth) - 10, 1, 100));
                context->annotatePrefs()->setLineWidth(_p->lineWidth);
            });
            connect(
                _p->actions->action(AnnotateActions::UNDO),
                &QAction::triggered,
                [this]
            {

            });
            connect(
                _p->actions->action(AnnotateActions::REDO),
                &QAction::triggered,
                [this]
            {

            });
            connect(
                _p->actions->action(AnnotateActions::CLEAR),
                &QAction::triggered,
                [this]
            {
                clearDrawing();
            });
            connect(
                _p->actions->action(AnnotateActions::NEW),
                &QAction::triggered,
                [this]
            {
                newAnnotation();
            });
            connect(
                _p->actions->action(AnnotateActions::DELETE),
                &QAction::triggered,
                [this]
            {
                UI::QuestionDialog dialog(
                    qApp->translate("djv::ViewLib::AnnotateGroup", "Are you sure you want to delete the current annotation?"));
                if (QDialog::Accepted == dialog.exec())
                {
                    deleteAnnotation();
                }
            });
            connect(
                _p->actions->action(AnnotateActions::DELETE_ALL),
                &QAction::triggered,
                [this]
            {
                UI::QuestionDialog dialog(
                    qApp->translate("djv::ViewLib::AnnotateGroup", "Are you sure you want to delete all annotations?"));
                if (QDialog::Accepted == dialog.exec())
                {
                    deleteAllAnnotations();
                }
            });
            connect(
                _p->actions->action(AnnotateActions::NEXT),
                &QAction::triggered,
                [this]
            {
                nextAnnotation();
            });
            connect(
                _p->actions->action(AnnotateActions::PREV),
                &QAction::triggered,
                [this]
            {
                prevAnnotation();
            });

            connect(
                _p->actions->group(AnnotateActions::PRIMITIVE_GROUP),
                &QActionGroup::triggered,
                [this, context](QAction * action)
            {
                _p->primitive = static_cast<Enum::ANNOTATE_PRIMITIVE>(action->data().toInt());
                context->annotatePrefs()->setPrimitive(_p->primitive);
            });

            _p->actions->action(AnnotateActions::SHOW)->connect(
                _p->annotateToolDockWidget,
                SIGNAL(visibilityChanged(bool)),
                SLOT(setChecked(bool)));

            auto fileGroup = session->fileGroup();
            connect(
                fileGroup,
                &FileGroup::fileInfoChanged,
                [this, context](const FileInfo & value)
            {
                if (doSave())
                {
                    saveAnnotations();
                }
                deleteAllAnnotations();
                setSummary(QString());
                _p->fileInfo = value;
                _p->jsonFileInfo = FileInfo();
                _p->exportFileInfo = FileInfo();
                if (!value.isEmpty())
                {
                    _p->jsonFileInfo.setPath(value.path());
                    _p->jsonFileInfo.setBase(cleanBaseName(value.base()));
                    _p->jsonFileInfo.setExtension(jsonExtension);
                    _p->jsonFileInfo.stat();

                    _p->exportFileInfo.setPath(value.path());
                    auto tmp = cleanBaseName(value.base());
                    if (!tmp.isEmpty())
                    {
                        tmp.append(".");
                    }
                    tmp.append(exportSuffix);
                    _p->exportFileInfo.setBase(tmp);
                    _p->exportFileInfo.setNumber("0");
                    _p->exportFileInfo.setExtension(context->annotatePrefs()->exportExtension());

                    loadAnnotations();
                }
                Q_EMIT exportChanged(_p->exportFileInfo);
            });

            auto playbackGroup = session->playbackGroup();
            connect(
                playbackGroup,
                &PlaybackGroup::sequenceChanged,
                [this](const Sequence & value)
            {
                _p->sequence = value;
            });
            connect(
                playbackGroup,
                &PlaybackGroup::speedChanged,
                [this](const Speed & value)
            {
                _p->speed = value;
            });
            connect(
                playbackGroup,
                &PlaybackGroup::frameChanged,
                [this](qint64 value)
            {
                _p->frameAnnotations.clear();
                Q_FOREACH(auto i, _p->annotations)
                {
                    if (i->frameIndex() == value)
                    {
                        _p->frameAnnotations.push_back(i);
                    }
                }
                const int index = _p->frameAnnotations.indexOf(_p->currentAnnotation);
                if (-1 == index)
                {
                    if (_p->frameAnnotations.count())
                    {
                        _p->currentAnnotation = _p->frameAnnotations.back();
                    }
                    else
                    {
                        _p->currentAnnotation.clear();
                    }
                }
                update();
                Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
                Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
            });

            auto viewWidget = session->viewWidget();
            connect(
                viewWidget,
                SIGNAL(pickPressed(const glm::ivec2 &)),
                SLOT(pickPressedCallback(const glm::ivec2 &)));
            connect(
                viewWidget,
                SIGNAL(pickReleased(const glm::ivec2 &)),
                SLOT(pickReleasedCallback(const glm::ivec2 &)));
            connect(
                viewWidget,
                SIGNAL(pickMoved(const glm::ivec2 &)),
                SLOT(pickMovedCallback(const glm::ivec2 &)));

            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::primitiveChanged,
                [this](Enum::ANNOTATE_PRIMITIVE value)
            {
                _p->primitive = value;
                update();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::colorChanged,
                [this](const AV::Color & value)
            {
                _p->color = value;
                update();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::lineWidthChanged,
                [this](size_t value)
            {
                _p->lineWidth = value;
                update();
            });

            connect(
                context->stylePrefs(),
                &UI::StylePrefs::prefChanged,
                [this]
            {
                update();
            });
        }

        AnnotateGroup::~AnnotateGroup()
        {
            if (doSave())
            {
                saveAnnotations();
            }
        }

        const QList<Annotate::Data *> & AnnotateGroup::annotations() const
        {
            return _p->annotations;
        }

        const QList<Annotate::Data *> & AnnotateGroup::frameAnnotations() const
        {
            return _p->frameAnnotations;
        }

        Annotate::Data * AnnotateGroup::currentAnnotation() const
        {
            return _p->currentAnnotation;
        }

        bool AnnotateGroup::areAnnotationsVisible() const
        {
            return _p->actions->action(AnnotateActions::SHOW)->isChecked();
        }

        QPointer<QDockWidget> AnnotateGroup::annotateToolDockWidget() const
        {
            return _p->annotateToolDockWidget;
        }

        QPointer<QMenu> AnnotateGroup::createMenu() const
        {
            return new AnnotateMenu(_p->actions.data());
        }

        QPointer<QToolBar> AnnotateGroup::createToolBar() const
        {
            return new AnnotateToolBar(_p->actions.data(), context());
        }

        void AnnotateGroup::newAnnotation(const QString & text)
        {
            auto playbackGroup = session()->playbackGroup();
            playbackGroup->setPlayback(Enum::STOP);
            const qint64 frameIndex = playbackGroup->frame();
            qint64 frameNumber = 0;
            if (frameIndex >= 0 && frameIndex < static_cast<qint64>(_p->sequence.frames.count()))
            {
                frameNumber = _p->sequence.frames[frameIndex];
            }
            auto annotation = new Annotate::Data(frameIndex, frameNumber, text);
            connect(
                annotation,
                SIGNAL(primitivesChanged()),
                SLOT(update()));
            _p->annotations.push_back(annotation);
            qStableSort(_p->annotations.begin(), _p->annotations.end(),
                [](const Annotate::Data * a, const Annotate::Data * b)
            {
                return a && b ? (a->frameIndex() < b->frameIndex()) : 0;
            });
            _p->frameAnnotations.clear();
            Q_FOREACH(auto i, _p->annotations)
            {
                if (i->frameIndex() == frameIndex)
                {
                    _p->frameAnnotations.push_back(i);
                }
            }
            _p->currentAnnotation = annotation;
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
            Q_EMIT annotationsChanged(_p->annotations);
            Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
            Q_EMIT annotationAdded(annotation);
        }

        void AnnotateGroup::setCurrentAnnotation(Annotate::Data * value)
        {
            if (value == _p->currentAnnotation)
                return;
            _p->currentAnnotation = value;
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
        }

        void AnnotateGroup::setSummary(const QString & value)
        {
            if (value == _p->summary)
                return;
            _p->summary = value;
            Q_EMIT summaryChanged(_p->summary);
        }

        void AnnotateGroup::setExport(const Core::FileInfo& value)
        {
            if (value == _p->exportFileInfo)
                return;
            _p->exportFileInfo = value;
            Q_EMIT exportChanged(_p->exportFileInfo);
        }

        void AnnotateGroup::exportAnnotations()
        {
            saveAnnotations();
            session()->exportAnnotations(
                _p->annotations,
                _p->jsonFileInfo,
                _p->exportFileInfo,
                context()->annotatePrefs()->exportScript(),
                context()->annotatePrefs()->exportScriptOptions(),
                context()->annotatePrefs()->exportScriptInterpreter());
        }

        void AnnotateGroup::deleteAnnotation()
        {
            int index = _p->annotations.indexOf(_p->currentAnnotation);
            if (index != -1)
            {
                delete _p->annotations[index];
                _p->annotations.removeAt(index);
                _p->frameAnnotations.clear();
                const qint64 frameIndex = session()->playbackGroup()->frame();
                Q_FOREACH(auto i, _p->annotations)
                {
                    if (i->frameIndex() == frameIndex)
                    {
                        _p->frameAnnotations.push_back(i);
                    }
                }
                if (index >= _p->annotations.size())
                {
                    --index;
                }
                if (index >= 0 && index < _p->annotations.size())
                {
                    _p->currentAnnotation = _p->annotations[index];
                }
                update();
                Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
                Q_EMIT annotationsChanged(_p->annotations);
                Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
            }
        }

        void AnnotateGroup::deleteAllAnnotations()
        {
            Q_FOREACH(auto i, _p->annotations)
            {
                delete i;
            }
            _p->annotations.clear();
            _p->frameAnnotations.clear();
            _p->currentAnnotation.clear();
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
            Q_EMIT annotationsChanged(_p->annotations);
            Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
        }

        void AnnotateGroup::nextAnnotation()
        {
            int index = _p->annotations.indexOf(_p->currentAnnotation);
            ++index;
            if (index >= _p->annotations.size())
            {
                index = 0;
            }
            if (index >= 0 && index < _p->annotations.count())
            {
                _p->currentAnnotation = _p->annotations[index];
            }
            else
            {
                _p->currentAnnotation.clear();
            }
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
        }

        void AnnotateGroup::prevAnnotation()
        {
            int index = _p->annotations.indexOf(_p->currentAnnotation);
            --index;
            if (index < 0)
            {
                index = _p->annotations.count() - 1;
            }
            if (index >= 0 && index < _p->annotations.count())
            {
                _p->currentAnnotation = _p->annotations[index];
            }
            else
            {
                _p->currentAnnotation.clear();
            }
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
        }

        void AnnotateGroup::undoDrawing()
        {

        }

        void AnnotateGroup::redoDrawing()
        {

        }

        void AnnotateGroup::clearDrawing()
        {
            if (_p->currentAnnotation)
            {
                UI::QuestionDialog dialog(
                    qApp->translate("djv::ViewLib::AnnotateGroup", "Are you sure you want to clear the drawing?"));
                if (QDialog::Accepted == dialog.exec())
                {
                    _p->currentAnnotation->clearPrimitives();
                }
            }
        }

        void AnnotateGroup::loadAnnotations()
        {
            if (_p->jsonFileInfo.exists())
            {
                try
                {
                    FileIO fileIO;
                    fileIO.open(_p->jsonFileInfo.fileName(), FileIO::MODE::READ);
                    picojson::value v;
                    const char * p = reinterpret_cast<const char *>(fileIO.mmapP());
                    const char * end = reinterpret_cast<const char *>(fileIO.mmapEnd());
                    std::string error;
                    picojson::parse(v, p, end, &error);
                    if (!error.empty())
                    {
                        throw Error(QString::fromStdString(error));
                    }
                    for (const auto & i : v.get<picojson::object>())
                    {
                        if ("export" == i.first)
                        {
                            std::string s;
                            djv::fromJSON(i.second, s);
                            _p->exportFileInfo = QString::fromStdString(s);
                            Q_EMIT exportChanged(_p->exportFileInfo);
                        }
                        else if ("frames" == i.first)
                        {
                            for (const auto & j : i.second.get<picojson::array>())
                            {
                                qint64 frameIndex = 0;
                                qint64 frameNumber = 0;
                                QString text;
                                std::vector<std::shared_ptr<Annotate::AbstractPrimitive> > primitives;
                                for (const auto & k : j.get<picojson::object>())
                                {
                                    if ("frameIndex" == k.first)
                                    {
                                        int v = 0;
                                        djv::fromJSON(k.second, v);
                                        frameIndex = v;
                                    }
                                    if ("frameNumber" == k.first)
                                    {
                                        int v = 0;
                                        djv::fromJSON(k.second, v);
                                        frameNumber = v;
                                    }
                                    else if ("text" == k.first)
                                    {
                                        std::string s;
                                        djv::fromJSON(k.second, s);
                                        text = QString::fromStdString(s);
                                    }
                                    else if ("primitives" == k.first)
                                    {
                                        for (const auto & l : k.second.get<picojson::array>())
                                        {
                                            std::shared_ptr<Annotate::AbstractPrimitive> primitive;
                                            for (const auto & m : l.get<picojson::object>())
                                            {
                                                if ("type" == m.first)
                                                {
                                                    std::string type;
                                                    djv::fromJSON(m.second, type);
                                                    if ("freehandLine" == type)
                                                    {
                                                        primitive = Annotate::create(Enum::ANNOTATE_FREEHAND_LINE);
                                                    }
                                                    else if ("line" == type)
                                                    {
                                                        primitive = Annotate::create(Enum::ANNOTATE_LINE);
                                                    }
                                                    else if ("rectangle" == type)
                                                    {
                                                        primitive = Annotate::create(Enum::ANNOTATE_RECTANGLE);
                                                    }
                                                    else if ("ellipse" == type)
                                                    {
                                                        primitive = Annotate::create(Enum::ANNOTATE_ELLIPSE);
                                                    }
                                                }
                                            }
                                            if (primitive)
                                            {
                                                primitive->fromJSON(l);
                                                primitives.push_back(primitive);
                                            }
                                        }
                                    }
                                }
                                auto data = new Annotate::Data(frameIndex, frameNumber, text);
                                data->setPrimitives(primitives);
                                _p->annotations.push_back(data);
                            }
                        }
                        else if ("summary" == i.first)
                        {
                            std::string s;
                            djv::fromJSON(i.second, s);
                            setSummary(QString::fromStdString(s));
                        }
                    }
                    qStableSort(_p->annotations.begin(), _p->annotations.end(),
                        [](const Annotate::Data * a, const Annotate::Data * b)
                    {
                        return a && b ? (a->frameIndex() < b->frameIndex()) : 0;
                    });
                    _p->frameAnnotations.clear();
                    auto playbackGroup = session()->playbackGroup();
                    const qint64 frameIndex = playbackGroup->frame();
                    Q_FOREACH(auto i, _p->annotations)
                    {
                        connect(
                            i,
                            SIGNAL(primitivesChanged()),
                            SLOT(update()));
                        if (i->frameIndex() == frameIndex)
                        {
                            _p->frameAnnotations.push_back(i);
                            _p->currentAnnotation = i;
                        }
                    }
                    update();
                    Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
                    Q_EMIT annotationsChanged(_p->annotations);
                    Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
                }
                catch (const Error & error)
                {
                    Error e(error);
                    e.add(QString("Cannot load annotations file \"%1\"").arg(_p->jsonFileInfo.fileName()));
                    context()->printError(e);
                }
                catch (const std::exception & error)
                {
                    Error e(error.what());
                    e.add(QString("Cannot load annotations file \"%1\"").arg(_p->jsonFileInfo.fileName()));
                    context()->printError(e);
                }
            }
        }

        void AnnotateGroup::saveAnnotations()
        {
            try
            {
                FileIO fileIO;
                fileIO.open(_p->jsonFileInfo.fileName(), FileIO::MODE::WRITE);
                picojson::value root(picojson::object_type, true);
                root.get<picojson::object>()["path"] = picojson::value(PicoJSON::escape(_p->fileInfo.fileName().toStdString()));
                root.get<picojson::object>()["export"] = picojson::value(PicoJSON::escape(_p->exportFileInfo.fileName().toStdString()));
                picojson::value frames(picojson::array_type, true);
                Q_FOREACH(auto i, _p->annotations)
                {
                    picojson::value frame(picojson::object_type, true);
                    QString s;
                    s.setNum(i->frameIndex());
                    frame.get<picojson::object>()["frameIndex"] = picojson::value(s.toStdString());
                    s.setNum(i->frameNumber());
                    frame.get<picojson::object>()["frameNumber"] = picojson::value(s.toStdString());
                    frame.get<picojson::object>()["text"] = picojson::value(PicoJSON::escape(i->text().toStdString()));
                    picojson::value primitives(picojson::array_type, true);
                    size_t k = 0;
                    for (const auto & j : i->primitives())
                    {
                        primitives.get<picojson::array>().push_back(j->toJSON());
                        ++k;
                    }
                    frame.get<picojson::object>()["primitives"] = primitives;
                    frames.get<picojson::array>().push_back(frame);
                }
                root.get<picojson::object>()["frames"] = frames;
                root.get<picojson::object>()["summary"] = picojson::value(PicoJSON::escape(_p->summary.toStdString()));
                PicoJSON::write(root, fileIO);
                fileIO.set("\n");
            }
            catch (const Error & error)
            {
                Error e(error);
                e.add(QString("Cannot save annotations file \"%1\"").arg(_p->jsonFileInfo.fileName()));
                context()->printError(e);
            }
            catch (const std::exception & error)
            {
                Error e(error.what());
                e.add(QString("Cannot save annotations file \"%1\"").arg(_p->jsonFileInfo.fileName()));
                context()->printError(e);
            }
        }

        void AnnotateGroup::pickPressedCallback(const glm::ivec2 & in)
        {
            if (_p->annotateToolDockWidget->isVisible())
            {
                if (!_p->currentAnnotation)
                {
                    newAnnotation();
                }
                if (_p->currentAnnotation)
                {
                    auto p = Annotate::create(_p->primitive);
                    p->setColor(_p->color);
                    p->setLineWidth(_p->lineWidth);
                    _p->currentAnnotation->addPrimitive(p);
                    _p->currentAnnotation->mouse(transformMousePos(in));
                }
            }
        }

        void AnnotateGroup::pickReleasedCallback(const glm::ivec2 & in)
        {
        }

        void AnnotateGroup::pickMovedCallback(const glm::ivec2 & in)
        {
            if (_p->annotateToolDockWidget->isVisible() && _p->currentAnnotation)
            {
                _p->currentAnnotation->mouse(transformMousePos(in));
            }
        }

        void AnnotateGroup::update()
        {
            const bool visible = _p->actions->action(AnnotateActions::SHOW)->isChecked();

            const int size = qApp->style()->pixelMetric(QStyle::PM_ButtonIconSize);
            QImage image(size, size, QImage::Format_ARGB32);
            image.fill(AV::ColorUtil::toQt(_p->color));
            _p->actions->action(AnnotateActions::COLOR)->setIcon(QPixmap::fromImage(image));
            _p->actions->action(AnnotateActions::COLOR)->setEnabled(visible);

            _p->actions->action(AnnotateActions::LINE_WIDTH_INC)->setEnabled(visible);
            _p->actions->action(AnnotateActions::LINE_WIDTH_DEC)->setEnabled(visible);
            _p->actions->action(AnnotateActions::UNDO)->setEnabled(visible);
            _p->actions->action(AnnotateActions::REDO)->setEnabled(visible);

            const int count = _p->annotations.count();
            _p->actions->action(AnnotateActions::CLEAR)->setEnabled(
                (_p->currentAnnotation ? _p->currentAnnotation->primitives().size() : false) && visible);
            _p->actions->action(AnnotateActions::NEW)->setEnabled(visible);
            _p->actions->action(AnnotateActions::DELETE)->setEnabled(_p->currentAnnotation && visible);
            _p->actions->action(AnnotateActions::DELETE_ALL)->setEnabled(count > 0 && visible);
            _p->actions->action(AnnotateActions::NEXT)->setEnabled(count > 1 && visible);
            _p->actions->action(AnnotateActions::PREV)->setEnabled(count > 1 && visible);

            _p->actions->group(AnnotateActions::PRIMITIVE_GROUP)->actions()[_p->primitive]->setChecked(true);
            _p->actions->group(AnnotateActions::PRIMITIVE_GROUP)->setEnabled(visible);

            _p->annotateToolDockWidget->setVisible(visible);
        }

        bool AnnotateGroup::doSave() const
        {
            bool out = false;
            if (!_p->jsonFileInfo.isEmpty())
            {
                out |= !_p->annotations.isEmpty();
                out |= !_p->summary.isEmpty();
                out |= _p->jsonFileInfo.exists();
            }
            return out;
        }

        glm::ivec2 AnnotateGroup::transformMousePos(const glm::ivec2 & value) const
        {
            const auto view = session()->viewWidget();
            const glm::ivec2 & viewPos = view->viewPos();
            const float viewZoom = view->viewZoom();
            return glm::ivec2(
                (value.x - viewPos.x) / viewZoom,
                (value.y - viewPos.y) / viewZoom);
        }

    } // namespace ViewLib
} // namespace djv
