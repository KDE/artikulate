/*
    SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "outputdevicecontroller.h"
#include "libsound_debug.h"
#include "outputbackendinterface.h"
#include "qtmultimediabackend/qtmultimediaoutputbackend.h"
#include <QUrl>

/**
 * \class OutputDeviceControllerPrivate
 * \internal
 *
 * This is the private data class for \see OutputDeviceController.
 * Note that -- even if the OutputDeviceController is constructed before its first call -- all
 * devices get only configured by first instantiation of OutputDeviceController with a call
 * to OutputDeviceControllerPrivate::lazyInit(), called in OutputDeviceController::self().
 */
class OutputDeviceControllerPrivate
{
public:
    OutputDeviceControllerPrivate(OutputDeviceController *parent)
        : m_backend(new QtMultimediaOutputBackend(parent))
        , m_volume(m_backend->volume())
    {
        parent->connect(m_backend.get(), &OutputBackendInterface::stateChanged, parent, &OutputDeviceController::emitChangedState);
    }

    OutputBackendInterface *backend() const
    {
        Q_ASSERT(m_backend);
        return m_backend.get();
    }

    std::unique_ptr<OutputBackendInterface> m_backend;
    int m_volume{0}; // volume as cubic value
};

OutputDeviceController::OutputDeviceController()
    : d(new OutputDeviceControllerPrivate(this))
{
}

OutputDeviceController::~OutputDeviceController()
{
}

OutputDeviceController &OutputDeviceController::self()
{
    static OutputDeviceController instance;
    return instance;
}

void OutputDeviceController::play(const QString &filePath)
{
    d->backend()->setUri(filePath);
    d->backend()->play();
    Q_EMIT started();
}

void OutputDeviceController::play(const QUrl &filePath)
{
    play(filePath.toLocalFile());
}

void OutputDeviceController::stop()
{
    d->backend()->stop();
    Q_EMIT stopped();
}

OutputDeviceController::State OutputDeviceController::state() const
{
    return d->backend()->state();
}

void OutputDeviceController::setVolume(int volume)
{
    // backend only accepts volume, when there is a pipeline
    // store value here and set it when playing
    d->backend()->setVolume(volume);
    d->m_volume = volume;
}

int OutputDeviceController::volume() const
{
    return d->backend()->volume();
}

void OutputDeviceController::emitChangedState()
{
    if (state() == OutputDeviceController::StoppedState) {
        Q_EMIT stopped();
        return;
    }
    if (state() == OutputDeviceController::PlayingState) {
        Q_EMIT started();
        return;
    }
}
