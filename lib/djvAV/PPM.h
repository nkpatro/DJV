//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/IO.h>

#include <djvCore/String.h>

#include <QOpenGLDebugLogger>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This plugin provides NetPBM image I/O.
            //!
            //! References:
            //! - Netpbm, "PPM Format Specification"
            //!   http://netpbm.sourceforge.net/doc/ppm.html
            namespace PPM
            {
                static const std::string pluginName = "PPM";
                static const std::set<std::string> fileExtensions = { ".ppm" };

                //! This enumeration provides the PPM data types.
                enum class Data
                {
                    ASCII,
                    Binary,

                    Count,
                    First = ASCII
                };
                DJV_ENUM_HELPERS(Data);

                //! Get the number of bytes in a scanline.
                size_t getScanlineByteCount(
                    int    width,
                    size_t channelCount,
                    size_t componentSize);

                //! Read ASCII data.
                void readASCII(
                    Core::FileIO& io,
                    uint8_t*      out,
                    size_t        size,
                    size_t        componentSize);

                //! Save ASCII data.
                size_t writeASCII(
                    const uint8_t* in,
                    char*          out,
                    size_t         size,
                    size_t         componentSize);

                class Read : public IRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    void _init(
                        const std::string & fileName,
                        const std::shared_ptr<Queue> &,
                        const std::shared_ptr<Core::Context> &);
                    Read();

                public:
                    ~Read();

                    static std::shared_ptr<Read> create(
                        const std::string & fileName,
                        const std::shared_ptr<Queue> &,
                        const std::shared_ptr<Core::Context> &);

                    std::future<Info> getInfo() override;

                private:
                    Core::FileIO _open(const std::string &);

                    DJV_PRIVATE();
                };
                
                class Write : public IWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    void _init(
                        const std::string & fileName,
                        const Info &,
                        Data,
                        const std::shared_ptr<Queue> &,
                        const std::shared_ptr<Core::Context> &);
                    Write();

                public:
                    ~Write();

                    static std::shared_ptr<Write> create(
                        const std::string & fileName,
                        const Info &,
                        Data,
                        const std::shared_ptr<Queue> &,
                        const std::shared_ptr<Core::Context> &);

                protected:
                    void run() override;

                private Q_SLOTS:
                    void _debugLogMessage(const QOpenGLDebugMessage &);

                private:
                    DJV_PRIVATE();
                };

                class Plugin : public IPlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    void _init(const std::shared_ptr<Core::Context>&);
                    Plugin();

                    picojson::value getOptions() const override;
                    void setOptions(const picojson::value&) override;

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<Core::Context>&);

                    std::shared_ptr<IRead> read(
                        const std::string & fileName,
                        const std::shared_ptr<Queue> &) const override;
                    std::shared_ptr<IWrite> write(
                        const std::string & fileName,
                        const Info &,
                        const std::shared_ptr<Queue> &) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace PNG
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::PPM::Data);

} // namespace djv