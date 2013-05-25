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

#include "capturedevicecontroller.h"
#include <settings.h>

#include <QMediaRecorder>
#include <QAudioInput>
#include <QAudioCaptureSource>
#include <QUrl>

#include <KDebug>

/**
 * \class CaptureDeviceControllerPrivate
 * \internal
 *
 * This is the private data class for \see CaptureDeviceController.
 * Note that -- even if the CaptureDeviceController is contructed before its first call -- all
 * devices get only configured by first instantiation of CaptureDeviceController with a call
 * to CaptureDeviceControllerPrivate::lazyInit(), called in CaptureDeviceController::self().
 */
class CaptureDeviceControllerPrivate
{
public:
    CaptureDeviceControllerPrivate(QObject *parent)
        : m_parent(parent)
        , m_initialized(false)
    {
    }

    ~CaptureDeviceControllerPrivate()
    {
        delete m_audioInput;
        delete m_captureSource;
    }

    void lazyInit()
    {
        if (m_initialized) {
            return;
        }

        // initialize
        m_captureSource = new QAudioCaptureSource(m_parent);
        m_audioInput = new QMediaRecorder(m_captureSource, m_parent);

        if (!m_captureSource->isAvailable()) {
            kError() << "Audio capture source not available.";
        }

        // check and set input device
        if (Settings::audioInputDevice().isEmpty()) {
            kDebug() << "Audio device not set, using default value.";
            Settings::setAudioInputDevice(m_captureSource->activeAudioInput());
        }
        if (m_captureSource->audioInputs().contains(Settings::audioInputDevice())) {
            m_captureSource->setAudioInput(Settings::audioInputDevice());
        } else {
            kError() << "Could not set audio input device, device " << Settings::audioInputDevice()
                << "is unknown. Using default device.";
        }

        // check code
        if (!m_audioInput->supportedAudioCodecs().contains("audio/vorbis")) {
            kError() << "Audio codec vorbis/ogg is not available, cannot record sound files.";
        }

        m_initialized = true;
    }

    QObject *m_parent;
    bool m_initialized;
    QMediaRecorder *m_audioInput;
    QAudioCaptureSource *m_captureSource;
};

CaptureDeviceController::CaptureDeviceController()
    : d(new CaptureDeviceControllerPrivate(this))
{
}

CaptureDeviceController::~CaptureDeviceController()
{
}

CaptureDeviceController & CaptureDeviceController::self()
{
    static CaptureDeviceController instance;
    instance.d->lazyInit();
    return instance;
}

void CaptureDeviceController::startCapture(const QString &filePath)
{
    if (d->m_audioInput->state() == QMediaRecorder::RecordingState) {
        kError() << "Currently recording, aborting record start";
        return;
    }

    // set output location
    //FIXME for a really strange reason, only the following notation works to get a correct
    // output file; neither QUrl::fromLocalFile, nor the KUrl equivalents are working
    // --> investigate why!
    d->m_audioInput->setOutputLocation(QUrl(filePath));

    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/vorbis");
    audioSettings.setSampleRate(0);
    audioSettings.setBitRate(0);
    audioSettings.setQuality(QtMultimediaKit::NormalQuality);
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    QString container = "ogg";

    d->m_audioInput->setEncodingSettings(audioSettings, QVideoEncoderSettings(), container);

    d->m_audioInput->record();
    emit captureStarted();
}

void CaptureDeviceController::stopCapture()
{
    d->m_audioInput->stop();
    emit captureStopped();
}

void CaptureDeviceController::setDevice(const QString &deviceIdentifier)
{
    if (!d->m_captureSource->audioInputs().contains(deviceIdentifier)) {
        kError() << "Audio input devicde " << deviceIdentifier << " is unknown, aborting.";
        return;
    }
    d->m_captureSource->setAudioInput(deviceIdentifier);
}

QMediaRecorder::State CaptureDeviceController::state() const
{
    return d->m_audioInput->state();
}

