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

#include <djv_info/InfoContext.h>

#include <djvCore/System.h>

#include <QCoreApplication>

namespace djv
{
    namespace info
    {
        Context::Context(int & argc, char ** argv, QObject * parent) :
            AV::AVContext(argc, argv, parent),
            _columns(Core::System::terminalWidth())
        {
            //DJV_DEBUG("Context::Context");

            // Load translators.
            loadTranslator("djv_info");
        }

        Context::~Context()
        {
            //DJV_DEBUG("Context::~Context");
        }

        const QStringList & Context::input() const
        {
            return _input;
        }

        bool Context::hasInfo() const
        {
            return _info;
        }

        bool Context::hasVerbose() const
        {
            return _verbose;
        }

        bool Context::hasFilePath() const
        {
            return _filePath;
        }

        bool Context::hasRecurse() const
        {
            return _recurse;
        }

        int Context::columns() const
        {
            return _columns;
        }

        bool Context::commandLineParse(QStringList & in)
        {
            //DJV_DEBUG("Context::commandLineParse");
            //DJV_DEBUG_PRINT("in = " << in);

            if (!AV::AVContext::commandLineParse(in))
                return false;

            QString arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;

                    // Parse the options.
                    if (
                        qApp->translate("djv::info::Context", "-x_info") == arg ||
                        qApp->translate("djv::info::Context", "-xi") == arg)
                    {
                        _info = false;
                    }
                    else if (
                        qApp->translate("djv::info::Context", "-verbose") == arg ||
                        qApp->translate("djv::info::Context", "-v") == arg)
                    {
                        _verbose = true;
                    }
                    else if (
                        qApp->translate("djv::info::Context", "-file_path") == arg ||
                        qApp->translate("djv::info::Context", "-fp") == arg)
                    {
                        _filePath = true;
                    }
                    else if (
                        qApp->translate("djv::info::Context", "-recurse") == arg ||
                        qApp->translate("djv::info::Context", "-r") == arg)
                    {
                        _recurse = true;
                    }
                    else if (
                        qApp->translate("djv::info::Context", "-columns") == arg ||
                        qApp->translate("djv::info::Context", "-c") == arg)
                    {
                        in >> _columns;
                    }

                    // Parse the arguments.
                    else
                    {
                        _input += arg;
                    }
                }
            }
            catch (const QString &)
            {
                throw QString(arg);
            }

            return true;
        }

        QString Context::commandLineHelp() const
        {
            static const QString label = qApp->translate("djv::info::Context",
                "djv_info\n"
                "\n"
                "    Print file metadata.\n"
                "\n"
                "    Example output:\n"
                "    yesterdayview.mov I[640x424:1.51 RGB U8] A[2:8@44100] T[00:02:00:01@12]\n"
                "    dlad.dpx                                      I[2048x1556:1.32 RGB U10]\n"
                "    render0001-1000.exr                                   T[00:00:41:16@24]\n"
                "        A,B,G,R                                     I[720x480:1.5 RGBA F16]\n"
                "        Ambient                                      I[720x480:1.5 RGB F16]\n"
                "        Diffuse                                      I[720x480:1.5 RGB F16]\n"
                "        Specular                                     I[720x480:1.5 RGB F16]\n"
                "        Z                                              I[720x480:1.5 L F32]\n"
                "\n"
                "    Key:\n"
                "    yesterdayview.mov I[640x424:1.51 RGB U8] A[2:8@44100] T[00:02:00:01@12]\n"
                "    (File Name)       (Image)                (Audio)      (Time)\n"
                "\n"
                "    Image - (width)x(height):(aspect) (format) (type)\n"
                "    Audio - (channels):(bit depth)@(sample rate)\n"
                "    Time  - (time)@(speed)\n"
                "\n"
                "Usage\n"
                "\n"
                "    djv_info [file|directory]... [option]...\n"
                "\n"
                "    file      - One or more files or file sequences\n"
                "    directory - One or more directories\n"
                "\n"
                "    If no files or directories are given then the current directory will be "
                "used.\n"
                "\n"
                "Options\n"
                "\n"
                "    -x_info, -xi\n"
                "        Don't show information, only file names.\n"
                "    -verbose, -v\n"
                "        Show verbose information.\n"
                "    -file_path, -fp\n"
                "        Show file path names.\n"
                "    -recurse, -r\n"
                "        Recursively descend into sub-directories.\n"
                "    -columns, -c (value)\n"
                "        Set the number of columns used to format the output. "
                "Setting this value to zero disables formatting.\n"
                "%1"
                "\n"
                "Examples\n"
                "\n"
                "    Display information:\n"
                "    > djv_info image.sgi image2.sgi\n"
                "\n"
                "    Display sequence information:\n"
                "    > djv_info image.1-100.sgi\n"
                "\n"
                "    Display information about all files within a directory:\n"
                "    > djv_info ~/pics\n");
            return QString(label).
                arg(AV::AVContext::commandLineHelp());
        }
    
    } // namespace info
} // namespace djv
