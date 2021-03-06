//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#include <BlurOp.h>

#include <Context.h>

#include <djvUI/IntEditSlider.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/OpenGLImage.h>

#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>

using namespace gl;

//------------------------------------------------------------------------------
// BlurOp::Values
//------------------------------------------------------------------------------

BlurOp::Values::Values() :
    radius(3),
    filter(GAUSSIAN)
{}

bool BlurOp::Values::operator == (const Values & other) const
{
    return
        other.radius == radius &&
        other.filter == filter;
}

//------------------------------------------------------------------------------
// BlurOp::State
//------------------------------------------------------------------------------

BlurOp::State::State() :
    _init(false)
{}

BlurOp::State::State(const Values & in) :
    _values(in),
    _init  (true)
{}

bool BlurOp::State::operator != (const State & in) const
{
    return
        _values.radius != in._values.radius ||
        _values.filter != in._values.filter ||
        ! _init;
}

//------------------------------------------------------------------------------
// BlurOp
//------------------------------------------------------------------------------

const QStringList & BlurOp::filterLabels()
{
    static const QStringList data = QStringList() <<
        "Box" <<
        "Gaussian";
    return data;
}

BlurOp::BlurOp(Context * context) :
    AbstractOp(context)
{}

const BlurOp::Values & BlurOp::values() const
{
    return _values;
}

namespace
{
const QString vertexSource =
"void main(void)\n"
"{\n"
"    gl_FrontColor  = gl_Color;\n"
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"}\n";

const QString fragmentSource =
"%1"
"\n"
"uniform sampler2DRect texture;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = texture;\n"
"}\n";

} // namespace

void BlurOp::render(const djvImage & in) throw (djvError)
{
    DJV_DEBUG("BlurOp::render");
    DJV_DEBUG_PRINT("in = " << in);

    // Initialize.
    begin();
    _texture.init(in, GL_TEXTURE_RECTANGLE);
    _render.textureTmp.init(
        djvPixelDataInfo(in.size(), in.pixel()),
        GL_TEXTURE_RECTANGLE);

    const int size = _values.radius * 2 + 1;
    const State state(_values);
    if (_state != state)
    {
        _render.offscreen.init();
        _render.kernel.init(size);
        _render.shader.init(
            vertexSource,
            QString(fragmentSource).arg(_render.kernel.src()));
        _render.shader.bind();

        // Kernel weights.
        std::vector<float> value(size);
        float * p = value.data();
        switch (_values.filter)
        {
            case BOX:
                for (int i = 0; i < size; ++i)
                {
                    p[i] = 1;
                }
                break;
            case GAUSSIAN:
            {
                const float theta = size / 6.f;
                float x = -_values.radius;
                for (int i = 0; i < size; ++i, x += 1.f)
                {
                    p[i] = static_cast<float>(
                        djvMath::sqrt(1.f / (djvMath::sqrt(2.f * djvMath::pi * theta))) *
                        djvMath::exp(-(x * x) / (2 * theta * theta)));
                }
            }
            break;
        }
        float sum = 0.f;
        for (int i = 0; i < size; ++i)
        {
            sum += p[i];
        }
        for (int i = 0; i < size; ++i)
        {
            //DJV_DEBUG_PRINT("i = " << p[i]);
            p[i] = p[i] / sum;
        }
        _render.kernel.value(_render.shader.program(), value.data());
        _state = state;
    }

    // Horizontal.
    _render.offscreen.bind();
    _render.offscreen.set(_render.textureTmp);
    _render.shader.bind();
    const GLuint progam = _render.shader.program();
    std::vector<float> offset(size * 2);
    memset(offset.data(), 0, offset.size() * sizeof(float));
    float * p = offset.data();
    for (int i = -_values.radius; i <= _values.radius; ++i, p += 2)
    {
        p[0] = static_cast<float>(i);
    }
    _render.kernel.offset(progam, offset.data());
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(progam, "texture"), 0);
    _texture.bind();
    const djvPixelDataInfo & info = in.info();
    djvOpenGLUtil::ortho(info.size);
    glViewport(0, 0, info.size.x, info.size.y);
    Util::quad(info);
    _render.offscreen.unbind();

    // Vertical.
    memset(offset.data(), 0, offset.size() * sizeof(float));
    p = offset.data();
    for (int i = -_values.radius; i <= _values.radius; ++i, p += 2)
    {
        p[1] = static_cast<float>(i);
    }
    _render.kernel.offset(progam, offset.data());
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(progam, "texture"), 0);
    _render.textureTmp.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    Util::quad(djvPixelDataInfo(info.size, info.pixel));

    end();
}

void BlurOp::setValues(const Values & values)
{
    if (values == _values)
        return;
    _values = values;
    Q_EMIT opChanged();
}

//------------------------------------------------------------------------------
// BlurOpWidget
//------------------------------------------------------------------------------

BlurOpWidget::BlurOpWidget(
    BlurOp *  op,
    Context * context) :
    AbstractOpWidget(context),
    _op(op)
{
    // Create the widgets.
    QGroupBox * radiusGroup = new QGroupBox("Radius");

    djvIntEditSlider * radius = new djvIntEditSlider(context);
    radius->setRange(0, 127);
    radius->setDefaultValue(0);

    QGroupBox * filterGroup = new QGroupBox("Filter");

    QComboBox * filter = new QComboBox;
    filter->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    filter->addItems(BlurOp::filterLabels());

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);

    QVBoxLayout * vLayout = new QVBoxLayout(radiusGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(radius);
    layout->addWidget(radiusGroup);

    vLayout = new QVBoxLayout(filterGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(filter);
    layout->addWidget(filterGroup);

    layout->addStretch();

    // Initialize.
    radius->setValue(op->values().radius);
    filter->setCurrentIndex(op->values().filter);

    // Setup the callbacks.
    connect(radius, SIGNAL(valueChanged(int)), SLOT(radiusCallback(int)));
    connect(filter, SIGNAL(currentIndexChanged(int)), SLOT(filterCallback(int)));
}

void BlurOpWidget::radiusCallback(int in)
{
    BlurOp::Values values = _op->values();
    values.radius = in;
    _op->setValues(values);
}

void BlurOpWidget::filterCallback(int in)
{
    BlurOp::Values values = _op->values();
    values.radius = static_cast<BlurOp::FILTER>(in);
    _op->setValues(values);
}

//------------------------------------------------------------------------------
// BlurOpFactory
//------------------------------------------------------------------------------

BlurOpFactory::BlurOpFactory(Context * context) :
    AbstractOpFactory(context)
{}

AbstractOp * BlurOpFactory::createOp() const
{
    return new BlurOp(context());
}

AbstractOpWidget * BlurOpFactory::createWidget(AbstractOp * op) const
{
    return new BlurOpWidget(dynamic_cast<BlurOp *>(op), context());
}

