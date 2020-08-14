/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef QTGSTREAMERCAPTUREBACKEND_H
#define QTGSTREAMERCAPTUREBACKEND_H

#include "capturebackendinterface.h"
#include "capturedevicecontroller.h"

#include <QString>

#include <QGst/Global>
#include <QGst/Pipeline>

class QtGStreamerCaptureBackend : public CaptureBackendInterface
{
    Q_OBJECT

public:
    QtGStreamerCaptureBackend();
    virtual ~QtGStreamerCaptureBackend();

    void startCapture(const QString &filePath);
    void stopCapture();
    CaptureDeviceController::State captureState() const;

    QStringList devices() const;
    void setDevice(const QString &deviceIdentifier);

private:
    void onBusMessage(const QGst::MessagePtr &message);
    void stopPipeline();
    QGst::BinPtr createAudioSrcBin();
    QGst::PipelinePtr m_pipeline;
    QString m_device;
    QMap<QString, QString> m_availableDevices; //!> (identifier,human readable name)
};

#endif
