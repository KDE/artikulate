/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "recorder.h"
#include "artikulate_debug.h"
#include "libsound/src/capturedevicecontroller.h"

#include <QDir>
#include <QString>

Recorder::Recorder(QObject *parent)
    : QObject(parent)
    , m_state(StoppedState)
    , m_recordingBufferFile(QDir::tempPath() + QStringLiteral("/XXXXXX.ogg"))
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
