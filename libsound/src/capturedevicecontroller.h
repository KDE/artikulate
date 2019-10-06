/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    ~CaptureDeviceController();

    const QScopedPointer<CaptureDeviceControllerPrivate> d;
};

#endif
