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

#include <OpsManager.h>

#include <BlurOp.h>
#include <ColorOp.h>
#include <Context.h>
#include <EdgeOp.h>
#include <ExposureOp.h>
#include <LevelsOp.h>
#include <NullOp.h>
#include <ScaleOp.h>
#include <SharpenOp.h>

//------------------------------------------------------------------------------
// OpsManager
//------------------------------------------------------------------------------

OpsManager::OpsManager(
    Context * context,
    QObject *            parent) :
    QObject(parent),
    _currentIndex(0)
{
    _factories += new NullOpFactory(context);
    _factories += new ColorOpFactory(context);
    _factories += new LevelsOpFactory(context);
    _factories += new ExposureOpFactory(context);
    _factories += new BlurOpFactory(context);
    _factories += new SharpenOpFactory(context);
    _factories += new EdgeOpFactory(context);
    _factories += new ScaleOpFactory(context);
    Q_FOREACH(AbstractOpFactory * factory, _factories)
    {
        _ops += factory->createOp();
    }
}

OpsManager::~OpsManager()
{
    Q_FOREACH(AbstractOp * op, _ops)
    {
        delete op;
    }
    Q_FOREACH(AbstractOpFactory * factory, _factories)
    {
        delete factory;
    }
}

const QList<AbstractOpFactory *> OpsManager::factories() const
{
    return _factories;
}

const QList<AbstractOp *> OpsManager::ops() const
{
    return _ops;
}

int OpsManager::currentIndex() const
{
    return _currentIndex;
}
    
AbstractOpFactory * OpsManager::currentFactory() const
{
    return _factories[_currentIndex];
}
    
AbstractOp * OpsManager::currentOp() const
{
    return _ops[_currentIndex];
}

void OpsManager::setCurrentIndex(int index)
{
    if (_currentIndex == index)
        return;
    disconnect(currentOp(), SIGNAL(opChanged()), this, SIGNAL(opChanged()));
    _currentIndex = index;
    connect(currentOp(), SIGNAL(opChanged()), SIGNAL(opChanged()));
    Q_EMIT currentIndexChanged(_currentIndex);
}
