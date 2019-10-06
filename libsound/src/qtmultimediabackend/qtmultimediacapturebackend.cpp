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

#include "qtmultimediacapturebackend.h"

#include "libsound_debug.h"

#include <QAudioRecorder>
#include <QUrl>

#include <KLocalizedString>

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
