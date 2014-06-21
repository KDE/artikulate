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
#include <KUrl>
#include <KDebug>

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
        , m_volume(0)
        , m_initialized(false)
    {
        // use this value only for initialization, will be modified in another thread / another
        // static Settings object
        //m_volume = Settings::audioOutputVolume(); //TODO currently not use anywhere
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
        m_initialized = true;
    }

    QtGStreamerOutputBackend * backend() const
    {
        Q_ASSERT(m_backend);
        return m_backend;
    }

    OutputDeviceController *m_parent;
    QtGStreamerOutputBackend *m_backend;
    int m_volume; // output volume in Db
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
//     d->m_audioOutput->setVolumeDecibel(d->m_volume); //TODO
    d->backend()->play();
    emit started();
}

void OutputDeviceController::play(const KUrl &filePath)
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
/*
QString OutputDeviceController::currentSource() const
{
    return d->m_mediaObject->currentSource().fileName();
}*/


void OutputDeviceController::setVolume(int volumenDb)
{
    d->m_volume = volumenDb;
}
