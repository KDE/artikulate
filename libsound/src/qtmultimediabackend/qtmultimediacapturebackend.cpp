/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "qtmultimediacapturebackend.h"
#include "libsound_debug.h"
#include <KLocalizedString>
#include <QUrl>
#include <QMediaFormat>
#include <QMediaDevices>

QtMultimediaCaptureBackend::QtMultimediaCaptureBackend(QObject *parent)
    : CaptureBackendInterface(parent)
{
    QMediaFormat mediaFormat;

    auto availableCodecs = mediaFormat.supportedAudioCodecs(QMediaFormat::Encode);
    if (availableCodecs.contains(QMediaFormat::AudioCodec::Vorbis)) {
        mediaFormat.setAudioCodec(QMediaFormat::AudioCodec::Vorbis);
    } else {
        qCWarning(LIBSOUND_LOG()) << "No vorbis codec was found for recordings";
        mediaFormat.setAudioCodec(availableCodecs.first());
    }
    qCDebug(LIBSOUND_LOG()) << "recording codec set to" << mediaFormat.audioCodec();

    m_recorder.setQuality(QMediaRecorder::HighQuality);
    m_recorder.setMediaFormat(mediaFormat);
    m_session.setRecorder(&m_recorder);
}

CaptureDeviceController::State QtMultimediaCaptureBackend::captureState() const
{
    switch (m_recorder.recorderState()) {
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

QList<QAudioDevice> QtMultimediaCaptureBackend::devices() const
{
    return QMediaDevices::audioInputs();
}

void QtMultimediaCaptureBackend::setDevice(QAudioInput &deviceIdentifier)
{
    if (devices().contains(deviceIdentifier.device())) {
        m_session.setAudioInput(&deviceIdentifier);
    } else {
        qCDebug(LIBSOUND_LOG) << "Could not set unknown capture device:" << deviceIdentifier.device().description();
    }
}
