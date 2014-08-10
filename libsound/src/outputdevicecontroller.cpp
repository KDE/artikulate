/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "outputdevicecontroller.h"
#include "qtgstreameroutputbackend.h"
#include <QUrl>
#include <QDebug>

/**
 * \class OutputDeviceControllerPrivate
 * \internal
 *
 * This is the private data class for \see OutputDeviceController.
 * Note that -- even if the OutputDeviceController is contructed before its first call -- all
 * devices get only configured by first instantiation of OutputDeviceController with a call
 * to OutputDeviceControllerPrivate::lazyInit(), called in OutputDeviceController::self().
 */
class OutputDeviceControllerPrivate
{
public:
    OutputDeviceControllerPrivate(OutputDeviceController *parent)
        : m_parent(parent)
        , m_backend(0)
        , m_initialized(false)
    {
        m_backend = new QtGStreamerOutputBackend();
    }

    ~OutputDeviceControllerPrivate()
    {
        delete m_backend;
    }

    void lazyInit()
    {
        if (m_initialized) {
            return;
        }
        m_backend = new QtGStreamerOutputBackend();
        m_parent->connect(m_backend, SIGNAL(stateChanged()), m_parent, SLOT(emitChangedState()));
        m_volume = m_backend->volume();
        m_initialized = true;
    }

    QtGStreamerOutputBackend * backend() const
    {
        Q_ASSERT(m_backend);
        return m_backend;
    }

    OutputDeviceController *m_parent;
    QtGStreamerOutputBackend *m_backend;
    int m_volume; // volume as cubic value
    bool m_initialized;
};

OutputDeviceController::OutputDeviceController()
    : d(new OutputDeviceControllerPrivate(this))
{

}

OutputDeviceController::~OutputDeviceController()
{
}

OutputDeviceController & OutputDeviceController::self()
{
    static OutputDeviceController instance;
    instance.d->lazyInit();
    return instance;
}

void OutputDeviceController::play(const QString& filePath)
{
    d->backend()->setUri(filePath);
    d->backend()->setVolume(d->m_volume);
    d->backend()->play();
    emit started();
}

void OutputDeviceController::play(const QUrl &filePath)
{
    play(filePath.toLocalFile());
}

void OutputDeviceController::stop()
{
    d->backend()->stop();
    emit stopped();
}

OutputDeviceController::State OutputDeviceController::state() const
{
    switch (d->backend()->state()) {
    case QGst::StateNull:
        return OutputDeviceController::StoppedState;
        break;
    case QGst::StatePaused:
        return OutputDeviceController::PlayingState;
        break;
    case QGst::StatePlaying:
        return OutputDeviceController::PlayingState;
        break;
    default:
        return OutputDeviceController::StoppedState;
    }
}

void OutputDeviceController::setVolume(int volume)
{
    // backend only accepts volume, when there is a pipeline
    // store value here and set it when playing
    d->m_volume = volume;
}

int OutputDeviceController::volume() const
{
    return d->backend()->volume();
}

void OutputDeviceController::emitChangedState()
{
    if (state() == OutputDeviceController::StoppedState) {
        emit stopped();
        return;
    }
    if (state() == OutputDeviceController::PlayingState) {
        emit started();
        return;
    }
}
