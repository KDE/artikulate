/*
    SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "capturedevicecontroller.h"
#include "capturebackendinterface.h"
#include "libsound_debug.h"
#include "qtmultimediabackend/qtmultimediacapturebackend.h"
#include <memory>

/**
 * \class CaptureDeviceControllerPrivate
 * \internal
 *
 * This is the private data class for \see CaptureDeviceController.
 * Note that -- even if the CaptureDeviceController is constructed before its first call -- all
 * devices get only configured by first instantiation of CaptureDeviceController with a call
 * to CaptureDeviceControllerPrivate::lazyInit(), called in CaptureDeviceController::self().
 */
class CaptureDeviceControllerPrivate
{
public:
    CaptureDeviceControllerPrivate(QObject *parent)
        : m_backend(new QtMultimediaCaptureBackend(parent))
    {
    }

    CaptureBackendInterface *backend() const
    {
        Q_ASSERT(m_backend);
        return m_backend.get();
    }

    std::unique_ptr<CaptureBackendInterface> m_backend;
};

CaptureDeviceController::CaptureDeviceController()
    : d(new CaptureDeviceControllerPrivate(this))
{
}

CaptureDeviceController::~CaptureDeviceController() = default;

CaptureDeviceController &CaptureDeviceController::self()
{
    static CaptureDeviceController instance;
    return instance;
}

void CaptureDeviceController::startCapture(const QString &filePath)
{
    d->backend()->startCapture(filePath);
    Q_EMIT captureStarted();
}

void CaptureDeviceController::stopCapture()
{
    d->backend()->stopCapture();
    Q_EMIT captureStopped();
}

void CaptureDeviceController::setDevice(QAudioInput &deviceIdentifier)
{
    d->backend()->setDevice(deviceIdentifier);
}

QList<QAudioDevice> CaptureDeviceController::devices() const
{
    return d->backend()->devices();
}

CaptureDeviceController::State CaptureDeviceController::state() const
{
    return d->backend()->captureState();
}
