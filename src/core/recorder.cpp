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

#include "recorder.h"
#include "libsound/src/capturedevicecontroller.h"

#include <QList>
#include "artikulate_debug.h"
#include <QDir>
#include <QUrl>
#include <QString>

Recorder::Recorder(QObject *parent)
    : QObject(parent)
    , m_state(StoppedState)
    , m_recordingBufferFile(QDir::tempPath() + QLatin1String("/XXXXXX.ogg"))
{

}

Recorder::~Recorder()
{
    // clear resources
    m_recordingBufferFile.close();
}

Recorder::CaptureState Recorder::state() const
{
    return m_state;
}

void Recorder::startCapture()
{
    if (CaptureDeviceController::self().state() == CaptureDeviceController::RecordingState) {
        qCWarning(ARTIKULATE_LOG) << "Stopped capture before starting new capture, since was still active.";
        CaptureDeviceController::self().stopCapture();
    }
    m_recordingBufferFile.open();
    qCDebug(ARTIKULATE_LOG) << "Start recording to temporary file " << m_recordingBufferFile.fileName();
    CaptureDeviceController::self().startCapture(m_recordingBufferFile.fileName());
    m_state = RecordingState;
    emit stateChanged();
}

void Recorder::stop()
{
    CaptureDeviceController::self().stopCapture();
    m_state = StoppedState;
    emit stateChanged();
    emit recordingFileChanged();
}

QString Recorder::recordingFile() const
{
    if (!m_recordingBufferFile.isOpen()) {
        return QString();
    }
    return m_recordingBufferFile.fileName();
}

void Recorder::storeToFile(const QString &path)
{
    if (m_recordingBufferFile.isOpen()) {
        QFile targetFile;
        targetFile.setFileName(path);
        if (!targetFile.exists() || targetFile.remove()) {
            m_recordingBufferFile.copy(path);
            m_recordingBufferFile.close();
            emit recordingFileChanged();
        } else {
            qCritical() << "Could not save buffered sound data to file, aborting.";
        }
    } else {
        qCritical() << "No buffer present.";
    }
}

void Recorder::clearBuffer()
{
    if (m_recordingBufferFile.isOpen()) {
        m_recordingBufferFile.close();
        emit recordingFileChanged();
    }
}
