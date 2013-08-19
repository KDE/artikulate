/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QTMULTIMEDIABACKEND_H
#define QTMULTIMEDIABACKEND_H

#include "soundbackendinterface.h"
#include <QMediaRecorder>

class QAudioCaptureSource;

class QtMultimediaBackend : public SoundBackendInterface
{
    Q_OBJECT

public:
    QtMultimediaBackend();

    virtual ~QtMultimediaBackend();

    virtual QString identifier();

    virtual void startCapture(const QString &filePath);
    virtual void stopCapture();
    virtual CaptureDeviceController::State captureState();

    virtual QStringList devices() const;
    virtual void setDevice(const QString &deviceIdentifier);

private:
    QMediaRecorder *m_audioInput;
    QAudioCaptureSource *m_captureSource;
    QString m_device;
};


#endif
