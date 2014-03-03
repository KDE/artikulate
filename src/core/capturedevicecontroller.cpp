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

#include "capturedevicecontroller.h"
#include "soundbackends/soundbackendinterface.h"
#include <config.h>

#if QTMOBILITY_FOUND
    #include "soundbackends/qtmultimediabackend.h"
#endif
#if QTGSTREAMER_FOUND
    #include "soundbackends/qtgstreamerbackend.h"
#endif

#include <settings.h>
#include <QUrl>
#include <KDebug>

/**
 * \class CaptureDeviceControllerPrivate
 * \internal
 *
 * This is the private data class for \see CaptureDeviceController.
 * Note that -- even if the CaptureDeviceController is contructed before its first call -- all
 * devices get only configured by first instantiation of CaptureDeviceController with a call
 * to CaptureDeviceControllerPrivate::lazyInit(), called in CaptureDeviceController::self().
 */
class CaptureDeviceControllerPrivate
{
public:
    CaptureDeviceControllerPrivate(QObject *parent)
        : m_parent(parent)
        , m_initialized(false)
    {
    }

    ~CaptureDeviceControllerPrivate()
    {
        for (int i = 0; i < m_backends.size(); ++i) {
            m_backends.at(i)->deleteLater();
        }
        m_backends.clear();
    }

    void lazyInit()
    {
        if (m_initialized) {
            return;
        }
    // add QtMobility as first backend
#if QTMOBILITY_FOUND
        kDebug() << "initilaize QtMultimediaBackend";
        m_backends.append(new QtMultimediaBackend());
#endif
#if QTGSTREAMER_FOUND
        kDebug() << "initilaize QtGStreamerBackend";
        m_backends.append(new QtGStreamerBackend());
#endif
        m_initialized = true;
    }

    SoundBackendInterface * backend() const
    {
        if (m_backends.isEmpty()) {
            return 0;
        }
        foreach (SoundBackendInterface *backend, m_backends) {
            if (Settings::recordingBackend() == backend->identifier()) {
                return backend;
            }
        }
        kDebug() << "could not understand backend configuration setting, default to first found";
        return m_backends.first();
    }

    QObject *m_parent;
    QList<SoundBackendInterface*> m_backends;
    bool m_initialized;
};

CaptureDeviceController::CaptureDeviceController()
    : d(new CaptureDeviceControllerPrivate(this))
{
}

CaptureDeviceController::~CaptureDeviceController()
{
}

CaptureDeviceController & CaptureDeviceController::self()
{
    static CaptureDeviceController instance;
    instance.d->lazyInit();
    return instance;
}

void CaptureDeviceController::startCapture(const QString &filePath)
{
    d->backend()->startCapture(filePath);
    emit captureStarted();
}

void CaptureDeviceController::stopCapture()
{
    d->backend()->stopCapture();
    emit captureStopped();
}

void CaptureDeviceController::setDevice(const QString &deviceIdentifier)
{
    d->backend()->setDevice(deviceIdentifier);
}

QList< QString > CaptureDeviceController::devices() const
{
    return d->backend()->devices();
}

CaptureDeviceController::State CaptureDeviceController::state() const
{
    return d->backend()->captureState();
}
