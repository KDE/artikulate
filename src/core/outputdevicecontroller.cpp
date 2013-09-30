/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include <settings.h>

#include <QUrl>
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
        , m_initialized(false)
    {
    }

    ~OutputDeviceControllerPrivate()
    {
        m_mediaObject->disconnect();
        m_mediaObject->deleteLater();
        m_audioOutput->deleteLater();
    }

    bool lazyInit()
    {
        if (m_initialized) {
            return false;
        }

        m_audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory);
        m_mediaObject = new Phonon::MediaObject();

        // initialize
        Phonon::createPath(m_mediaObject, m_audioOutput);

        m_initialized = true;
        return true;
    }

    OutputDeviceController *m_parent;
    bool m_initialized;
    Phonon::AudioOutput *m_audioOutput;
    Phonon::MediaObject *m_mediaObject;
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
    if (instance.d->lazyInit()) {
        connect(instance.d->m_mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
                &instance, SLOT(updateState()));
    }
    return instance;
}

void OutputDeviceController::play(const KUrl &filePath)
{
    d->m_mediaObject->setCurrentSource(filePath);
    d->m_audioOutput->setVolume(Settings::audioOutputVolume());
    d->m_mediaObject->play();

    emit started();
}

void OutputDeviceController::stop()
{
    d->m_mediaObject->stop();
    emit stopped();
}

Phonon::State OutputDeviceController::state() const
{
    return d->m_mediaObject->state();
}

QString OutputDeviceController::currentSource() const
{
    return d->m_mediaObject->currentSource().fileName();
}

void OutputDeviceController::updateState()
{
    if (d->m_mediaObject->state() == Phonon::StoppedState) {
        emit stopped();
    }
}

