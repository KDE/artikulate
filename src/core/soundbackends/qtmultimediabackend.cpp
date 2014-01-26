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

#include "qtmultimediabackend.h"
#include "settings.h"

#include <QMediaRecorder>
#include <QAudioInput>
#include <QAudioCaptureSource>

#include <KDebug>

QtMultimediaBackend::QtMultimediaBackend()
{
    // initialize
    m_captureSource = new QAudioCaptureSource(this);
    m_audioInput = new QMediaRecorder(m_captureSource, this);

    if (!m_captureSource->isAvailable()) {
        kError() << "Audio capture source not available.";
    }

    // capabilities of backend
    kDebug() << "Available devices:";
    foreach (const QString &device, m_captureSource->audioInputs()) {
        kDebug() << " + " << device;
    }
    kDebug() << "Available codecs:";
    foreach(const QString &codecName, m_audioInput->supportedAudioCodecs()) {
        kDebug() << " + " << codecName;
    }
    kDebug() << "Available containers:";
    foreach(const QString &containerName, m_audioInput->supportedContainers()) {
        kDebug() << " + " << containerName;
    }

    // set input device
    if (!Settings::audioInputDevice().isEmpty()) {
        setDevice(Settings::audioInputDevice());
    } else {
        kDebug() << "No audio input setting available, default to first in device list";
        setDevice(devices().first());
    }

    // check code
    if (!m_audioInput->supportedAudioCodecs().contains("audio/vorbis")) {
        kError() << "Audio codec vorbis/ogg is not available, cannot record sound files.";
    }
}

QtMultimediaBackend::~QtMultimediaBackend()
{
    m_audioInput->deleteLater();
    m_captureSource->deleteLater();
}

QString QtMultimediaBackend::identifier()
{
    return QLatin1String("qtmultimediabackend");
}

CaptureDeviceController::State QtMultimediaBackend::captureState()
{
    switch (m_audioInput->state()) {
    case QMediaRecorder::StoppedState:
        return CaptureDeviceController::StoppedState;
        break;
    case QMediaRecorder::PausedState:
        return CaptureDeviceController::PausedState;
        break;
    case QMediaRecorder::QMediaRecorder::RecordingState:
        return CaptureDeviceController::RecordingState;
        break;
    default:
        return CaptureDeviceController::StoppedState;
    }
}

void QtMultimediaBackend::startCapture(const QString &filePath)
{
    if (m_audioInput->state() == QMediaRecorder::RecordingState) {
        kError() << "Currently recording, aborting record start";
        return;
    }

    kDebug() << "starting capture for " << filePath;
    kDebug() << "selected device " << m_device;

    // set output location
    //FIXME for a really strange reason, only the following notation works to get a correct
    // output file; neither QUrl::fromLocalFile, nor the KUrl equivalents are working
    // --> investigate why!
    m_audioInput->setOutputLocation(QUrl(filePath));
    m_captureSource->setAudioInput(m_device);

    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/vorbis");
    audioSettings.setSampleRate(0);
    audioSettings.setBitRate(0);
    audioSettings.setQuality(QtMultimediaKit::NormalQuality);
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    QString container = "ogg";

    m_audioInput->setEncodingSettings(audioSettings, QVideoEncoderSettings(), container);
    m_audioInput->record();
    if (m_audioInput->error() != QMediaRecorder::NoError) {
        kError() << "QtMultimediaBackend returned error: " << m_audioInput->errorString();
    }
}

void QtMultimediaBackend::stopCapture()
{
    if (m_audioInput->state() != QMediaRecorder::RecordingState) {
        kError() << "Could not stop capture, was not in recording state.";
    }
    m_audioInput->stop();
    kDebug() << "capture stopped";
}

QStringList QtMultimediaBackend::devices() const
{
    return m_captureSource->audioInputs();
}

void QtMultimediaBackend::setDevice(const QString &deviceIdentifier)
{
    if (!m_captureSource->audioInputs().contains(deviceIdentifier)) {
        kError() << "Audio input device " << deviceIdentifier << " is unknown, aborting.";
        return;
    }
    m_device = deviceIdentifier;
}
