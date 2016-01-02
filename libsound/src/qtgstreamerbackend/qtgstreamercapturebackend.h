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

#ifndef QTGSTREAMERCAPTUREBACKEND_H
#define QTGSTREAMERCAPTUREBACKEND_H

#include "capturedevicecontroller.h"
#include "capturebackendinterface.h"

#include <QString>

#include <QGst/global.h>
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
