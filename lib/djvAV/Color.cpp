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

#include <djvAV/Color.h>

#include <djvAV/ColorUtil.h>

#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/Memory.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        void Color::init()
        {
            _channels = Pixel::channels(_pixel);
        }

        Color::Color()
        {
            init();
            zero();
        }

        Color::Color(const Color & in)
        {
            init();
            *this = in;
        }

        Color::Color(Pixel::PIXEL pixel) :
            _pixel(pixel)
        {
            init();
            zero();
        }

        Color::Color(Pixel::F32_T l) :
            _pixel(Pixel::L_F32)
        {
            init();
            setF32(l, 0);
        }

        Color::Color(Pixel::F32_T l, Pixel::F32_T a) :
            _pixel(Pixel::LA_F32)
        {
            init();
            setF32(l, 0);
            setF32(a, 1);
        }

        Color::Color(Pixel::F32_T r, Pixel::F32_T g, Pixel::F32_T b) :
            _pixel(Pixel::RGB_F32)
        {
            init();
            setF32(r, 0);
            setF32(g, 1);
            setF32(b, 2);
        }

        Color::Color(Pixel::F32_T r, Pixel::F32_T g, Pixel::F32_T b, Pixel::F32_T a) :
            _pixel(Pixel::RGBA_F32)
        {
            init();
            setF32(r, 0);
            setF32(g, 1);
            setF32(b, 2);
            setF32(a, 3);
        }

        Color::Color(const quint8 * data, Pixel::PIXEL pixel) :
            _pixel(pixel)
        {
            init();
            memcpy(_data, data, Pixel::byteCount(_pixel));
        }

        void Color::setPixel(Pixel::PIXEL pixel)
        {
            if (pixel == _pixel)
                return;
            Color tmp(pixel);
            ColorUtil::convert(*this, tmp);
            *this = tmp;
        }

        void Color::zero()
        {
            Core::Memory::fill<quint8>(0, _data, Pixel::byteCount(_pixel));
        }

        Color & Color::operator = (const Color & in)
        {
            if (&in != this)
            {
                _pixel = in._pixel;
                _channels = in._channels;
                memcpy(_data, in._data, Pixel::byteCount(_pixel));
            }
            return *this;
        }

    } // namespace AV

    bool operator == (const AV::Color & a, const AV::Color & b)
    {
        const AV::Pixel::PIXEL pixel = a.pixel();
        if (pixel != b.pixel())
            return false;
        const int channels = AV::Pixel::channels(pixel);
        for (int c = 0; c < channels; ++c)
        {
            switch (AV::Pixel::type(pixel))
            {
            case AV::Pixel::U8:
                if (a.u8(c) != b.u8(c))
                    return false;
                break;
            case AV::Pixel::U10:
                if (a.u10(c) != b.u10(c))
                    return false;
                break;
            case AV::Pixel::U16:
                if (a.u16(c) != b.u16(c))
                    return false;
                break;
            case AV::Pixel::F16:
                if (a.f16(c) != b.f16(c))
                    return false;
                break;
            case AV::Pixel::F32:
                if (!Core::Math::fuzzyCompare(a.f32(c), b.f32(c)))
                    return false;
                break;
            default: break;
            }
        }
        return true;
    }

    bool operator != (const AV::Color & a, const AV::Color & b)
    {
        return !(a == b);
    }

    QStringList & operator >> (QStringList & in, AV::Color & out)
    {
        AV::Pixel::PIXEL pixel = static_cast<AV::Pixel::PIXEL>(0);
        in >> pixel;
        out.setPixel(pixel);
        const int channels = AV::Pixel::channels(pixel);
        for (int c = 0; c < channels; ++c)
        {
            switch (AV::Pixel::type(pixel))
            {
            case AV::Pixel::U8:
            {
                int value = 0;
                in >> value;
                out.setU8(Core::Math::clamp(value, 0, AV::Pixel::u8Max), c);
            }
            break;
            case AV::Pixel::U10:
            {
                int value = 0;
                in >> value;
                out.setU10(Core::Math::clamp(value, 0, AV::Pixel::u10Max), c);
            }
            break;
            case AV::Pixel::U16:
            {
                int value = 0;
                in >> value;
                out.setU16(Core::Math::clamp(value, 0, AV::Pixel::u16Max), c);
            }
            break;
            case AV::Pixel::F16:
            {
                float value = 0.f;
                in >> value;
                out.setF16(static_cast<AV::Pixel::F16_T>(value), c);
            }
            break;
            case AV::Pixel::F32:
            {
                float value = 0.f;
                in >> value;
                out.setF32(static_cast<AV::Pixel::F32_T>(value), c);
            }
            break;
            default: break;
            }
        }
        return in;
    }

    QStringList & operator << (QStringList & out, const AV::Color & in)
    {
        out << in.pixel();
        const int channels = AV::Pixel::channels(in.pixel());
        for (int c = 0; c < channels; ++c)
        {
            switch (AV::Pixel::type(in.pixel()))
            {
            case AV::Pixel::U8:  out << in.u8(c); break;
            case AV::Pixel::U10: out << in.u10(c); break;
            case AV::Pixel::U16: out << in.u16(c); break;
            case AV::Pixel::F16: out << in.f16(c); break;
            case AV::Pixel::F32: out << in.f32(c); break;
            default: break;
            }
        }
        return out;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::Color & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    picojson::value toJSON(const AV::Color & value)
    {
        QStringList s;
        s << value;
        return picojson::value(s.join(' ').toStdString());
    }

    void fromJSON(const picojson::value & value, AV::Color & out)
    {
        if (value.is<std::string>())
        {
            auto s = QString::fromStdString(value.get<std::string>()).split(' ');
            if (s.count() >= 2)
            {
                const auto format = s.front();
                s.pop_front();
                s[0] = format + " " + s[0];
            }
            s >> out;
        }
        else
        {
            throw Core::Error(qApp->translate("djv::AV::Color", "Cannot parse the value."));
        }
    }

} // namespace djv
