/*
 *  SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef QTMULTIMEDIACAPTUREBACKEND_H
#define QTMULTIMEDIACAPTUREBACKEND_H

#include "capturebackendinterface.h"
#include "capturedevicecontroller.h"

#include <QAudioRecorder>
#include <QMap>
#include <QString>

class QAudioRecorder;
class QMediaRecorder;
class QMediaObject;

class QtMultimediaCaptureBackend : public CaptureBackendInterface
{
    Q_OBJECT

public:
    explicit QtMultimediaCaptureBackend(QObject *parent);
    ~QtMultimediaCaptureBackend() override = default;

    void startCapture(const QString &filePath) override;
    void stopCapture() override;
    CaptureDeviceController::State captureState() const override;

    QStringList devices() const override;
    void setDevice(const QString &deviceIdentifier) override;

private:
    QAudioRecorder m_recorder;
    QString m_device;
};

#endif
