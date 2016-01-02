/*
 *  Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef CAPTUREBACKENDINTERFACE_H
#define CAPTUREBACKENDINTERFACE_H

#include "capturedevicecontroller.h"
#include "libsound_export.h"
#include <QObject>

class LIBSOUND_EXPORT CaptureBackendInterface : public QObject
{
    Q_OBJECT

public:
    explicit CaptureBackendInterface(QObject *parent = nullptr);
    virtual ~CaptureBackendInterface();

    virtual void startCapture(const QString &filePath) = 0;
    virtual void stopCapture() = 0;
    virtual CaptureDeviceController::State captureState() const = 0;

    virtual QStringList devices() const = 0;
    virtual void setDevice(const QString &deviceIdentifier) = 0;
};


#endif

