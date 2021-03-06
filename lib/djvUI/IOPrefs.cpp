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

#include <djvUI/IOPrefs.h>

#include <djvUI/UIContext.h>
#include <djvUI/Prefs.h>

#include <djvAV/IO.h>

#include <QApplication>

namespace djv
{
    namespace UI
    {
        namespace
        {
            void _load(AV::IOPlugin * plugin)
            {
                //DJV_DEBUG("_load");
                //DJV_DEBUG_PRINT("plugin = " << plugin->pluginName());
                Prefs prefs(plugin->pluginName(), Prefs::SYSTEM);
                const QStringList & options = plugin->options();
                for (int i = 0; i < options.count(); ++i)
                {
                    QStringList tmp;
                    if (prefs.get(options[i], tmp))
                    {
                        //DJV_DEBUG_PRINT("tmp = " << tmp);
                        plugin->setOption(options[i], tmp);
                    }
                }
            }

            void _save(const AV::IOPlugin * plugin)
            {
                //DJV_DEBUG("_save");
                //DJV_DEBUG_PRINT("plugin = " << plugin->pluginName());
                Prefs prefs(plugin->pluginName(), Prefs::SYSTEM);
                const QStringList & options = plugin->options();
                for (int i = 0; i < options.count(); ++i)
                {
                    prefs.set(options[i], plugin->option(options[i]));
                }
            }

        } // namespace

        struct IOPrefs::Private
        {
            QPointer<UIContext> context;
        };

        IOPrefs::IOPrefs(const QPointer<UIContext> & context, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            _p->context = context;

            //DJV_DEBUG("IOPrefs::IOPrefs");
            const QList<Core::Plugin *> & plugins = context->ioFactory()->plugins();
            for (int i = 0; i < plugins.count(); ++i)
            {
                if (auto plugin = dynamic_cast<AV::IOPlugin *>(plugins[i]))
                {
                    _load(plugin);
                }
            }
        }

        IOPrefs::~IOPrefs()
        {
            //DJV_DEBUG("IOPrefs::~IOPrefs");
            const QList<Core::Plugin *> & plugins = _p->context->ioFactory()->plugins();
            for (int i = 0; i < plugins.count(); ++i)
            {
                if (auto plugin = dynamic_cast<AV::IOPlugin *>(plugins[i]))
                {
                    _save(plugin);
                }
            }
        }

    } // namespace UI
} // namespace djv
