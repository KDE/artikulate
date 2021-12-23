/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef CAPTUREDEVICECONTROLLER_H
#define CAPTUREDEVICECONTROLLER_H

#include "libsound_export.h"

#include <QObject>

class CaptureDeviceControllerPrivate;

/**
 * \class CaptureDeviceController
 *
 * This singleton class provides a controller for the sound capture device.
 */
class LIBSOUND_EXPORT CaptureDeviceController : public QObject
{
    Q_OBJECT

public:
    enum State { StoppedState, RecordingState, PausedState };

    /**
     * Returns self reference to the controller. First call of this method initializes
     * capture device controller.
     *
     * \return self reference
     */
    static CaptureDeviceController &self();

    void startCapture(const QString &filePath);
    CaptureDeviceController::State state() const;
    void stopCapture();
    void setDevice(const QString &deviceIdentifier);

    /**
     * \return list of available capture devices
     */
    QList<QString> devices() const;

public Q_SLOTS:

Q_SIGNALS:
    void captureStarted();
    void captureStopped();

private:
    Q_DISABLE_COPY(CaptureDeviceController)
    /**
     * \internal
     * Private constructor, \ref self().
     */
    CaptureDeviceController();

    /**
     * Private destructor.
     */
    ~CaptureDeviceController() override;

    const QScopedPointer<CaptureDeviceControllerPrivate> d;
};

#endif
