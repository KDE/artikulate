/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "qtmultimediacapturebackend.h"
#include "libsound_debug.h"
#include <KLocalizedString>
#include <QUrl>

QtMultimediaCaptureBackend::QtMultimediaCaptureBackend(QObject *parent)
    : CaptureBackendInterface(parent)
{
    QAudioEncoderSettings audioSettings;

    QStringList availableCodecs = m_recorder.supportedAudioCodecs();
    if (availableCodecs.contains(QLatin1String("audio/x-vorbis"))) {
        audioSettings.setCodec(QStringLiteral("audio/x-vorbis"));
    } else if (availableCodecs.contains(QLatin1String("audio/vorbis"))) {
        audioSettings.setCodec(QStringLiteral("audio/vorbis"));
    } else {
        qCWarning(LIBSOUND_LOG()) << "No vorbis codec was found for recordings";
        audioSettings.setCodec(availableCodecs.first());
    }
    qCDebug(LIBSOUND_LOG()) << "recording codec set to" << audioSettings.codec();

    audioSettings.setQuality(QMultimedia::HighQuality);
    m_recorder.setAudioSettings(audioSettings);
}

CaptureDeviceController::State QtMultimediaCaptureBackend::captureState() const
{
    switch (m_recorder.state()) {
    case QMediaRecorder::StoppedState:
        return CaptureDeviceController::StoppedState;
    case QMediaRecorder::RecordingState:
        return CaptureDeviceController::RecordingState;
    case QMediaRecorder::PausedState:
        return CaptureDeviceController::PausedState;
    }
    Q_UNREACHABLE();
    return CaptureDeviceController::StoppedState;
}

void QtMultimediaCaptureBackend::startCapture(const QString &filePath)
{
    m_recorder.setOutputLocation(QUrl::fromLocalFile(filePath));
    m_recorder.record();
}

void QtMultimediaCaptureBackend::stopCapture()
{
    m_recorder.stop();
}

QStringList QtMultimediaCaptureBackend::devices() const
{
    return m_recorder.audioInputs();
}

void QtMultimediaCaptureBackend::setDevice(const QString &deviceIdentifier)
{
    if (devices().contains(deviceIdentifier)) {
        m_recorder.setAudioInput(deviceIdentifier);
    } else {
        qCDebug(LIBSOUND_LOG) << "Could not set unknown capture device:" << deviceIdentifier;
    }
}
