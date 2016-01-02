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

#ifndef QTMULTIMEDIACAPTUREBACKEND_H
#define QTMULTIMEDIACAPTUREBACKEND_H

#include "capturedevicecontroller.h"
#include "capturebackendinterface.h"

#include <QMap>
#include <QString>

class QAudioRecorder;
class QMediaRecorder;
class QMediaObject;

class QtMultimediaCaptureBackend : public CaptureBackendInterface
{
    Q_OBJECT

public:
    QtMultimediaCaptureBackend();
    virtual ~QtMultimediaCaptureBackend();

    void startCapture(const QString &filePath);
    void stopCapture();
    CaptureDeviceController::State captureState() const;

    QStringList devices() const;
    void setDevice(const QString &deviceIdentifier);

private:
    QAudioRecorder *m_recorder;

    QString m_device;
};


#endif
