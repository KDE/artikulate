/*
 *  SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
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
