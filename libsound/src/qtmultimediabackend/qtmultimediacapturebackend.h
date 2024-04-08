/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef QTMULTIMEDIACAPTUREBACKEND_H
#define QTMULTIMEDIACAPTUREBACKEND_H

#include "capturebackendinterface.h"
#include "capturedevicecontroller.h"
#include <QMediaRecorder>
#include <QMediaCaptureSession>
#include <QList>
#include <QAudioDevice>

class QMediaRecorder;
class QMediaObject;
class QAudioInput;

class QtMultimediaCaptureBackend : public CaptureBackendInterface
{
    Q_OBJECT

public:
    explicit QtMultimediaCaptureBackend(QObject *parent);
    ~QtMultimediaCaptureBackend() override = default;

    void startCapture(const QString &filePath) override;
    void stopCapture() override;
    CaptureDeviceController::State captureState() const override;

    QList<QAudioDevice> devices() const override;
    void setDevice(QAudioInput &deviceIdentifier) override;

private:
    QMediaRecorder m_recorder;
    QMediaCaptureSession m_session;
    QString m_device;
};

#endif
