/*
    SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "qtgstreamercapturebackend.h"

#include "libsound_debug.h"
#include <KLocalizedString>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/Bus>
#include <QGst/ChildProxy>
#include <QGst/ElementFactory>
#include <QGst/Event>
#include <QGst/Init>
#include <QGst/Message>
#include <QGst/Pad>

QtGStreamerCaptureBackend::QtGStreamerCaptureBackend()
{
    QGst::init();

    // setup the device list
    QGst::ElementPtr src = QGst::ElementFactory::make("autoaudiosrc");

    if (!src) {
        qCritical() << "Failed to create element \"autoaudiosrc\". Make sure you have "
                    << "gstreamer-plugins-good installed";
        return;
    }

    m_availableDevices.insert("", i18nc("default sound device", "Default"));
}

QtGStreamerCaptureBackend::~QtGStreamerCaptureBackend()
{
    m_pipeline.clear();
}

CaptureDeviceController::State QtGStreamerCaptureBackend::captureState() const
{
    if (!m_pipeline) {
        return CaptureDeviceController::StoppedState;
    }

    switch (m_pipeline->currentState()) {
        case QGst::StateNull:
            return CaptureDeviceController::StoppedState;
            break;
        case QGst::StatePaused:
            return CaptureDeviceController::RecordingState;
            break;
        case QGst::StatePlaying:
            return CaptureDeviceController::RecordingState;
            break;
        default:
            return CaptureDeviceController::StoppedState;
    }
}

QGst::BinPtr QtGStreamerCaptureBackend::createAudioSrcBin()
{
    QGst::BinPtr audioBin;

    try {
        audioBin = QGst::Bin::fromDescription(
            "autoaudiosrc name=\"audiosrc\" ! audioconvert ! "
            "audioresample ! audiorate ! vorbisenc name=enc quality=0.6 ! queue");
    } catch (const QGlib::Error &error) {
        qCritical() << "Failed to create audio source bin:" << error;
        return QGst::BinPtr();
    }
    QGst::ElementPtr src = audioBin->getElementByName("audiosrc");
    // autoaudiosrc creates the actual source in the READY state

    src->setState(QGst::StateReady);
    return audioBin;
}

void QtGStreamerCaptureBackend::onBusMessage(const QGst::MessagePtr &message)
{
    switch (message->type()) {
        case QGst::MessageEos:
            // got end-of-stream - stop the pipeline
            qCDebug(LIBSOUND_LOG) << "EOS signal received, stopping pipeline";
            stopPipeline();
            break;
        case QGst::MessageError:
            // check if the pipeline exists before destroying it,
            // since we could get multiple error messages
            if (m_pipeline) {
                stopPipeline();
            }
            qCritical() << "Pipeline Error:" << message.staticCast<QGst::ErrorMessage>()->error().message();
            break;
        default:
            break;
    }
}

void QtGStreamerCaptureBackend::startCapture(const QString &filePath)
{
    // clear pipeline if still existing
    if (m_pipeline) {
        qCWarning(LIBSOUND_LOG) << "removing forgotten pipeline";
        // send an end-of-stream event to flush metadata and cause an EosMessage to be delivered
        m_pipeline->sendEvent(QGst::EosEvent::create());
    }

    QGst::BinPtr audioSrcBin = createAudioSrcBin();
    QGst::ElementPtr mux = QGst::ElementFactory::make("oggmux");
    QGst::ElementPtr sink = QGst::ElementFactory::make("filesink");

    if (!audioSrcBin || !mux || !sink) {
        qCritical() << "One or more elements could not be created. "
                    << "Verify that you have all the necessary element plugins installed.";
        return;
    }

    // set output path
    sink->setProperty("location", filePath);

    m_pipeline = QGst::Pipeline::create();
    m_pipeline->add(audioSrcBin, mux, sink);

    // link elements
    QGst::PadPtr audioPad = mux->getRequestPad("audio_%u");
    audioSrcBin->getStaticPad("src")->link(audioPad);

    mux->link(sink);

    // connect the bus
    m_pipeline->bus()->addSignalWatch();
    QGlib::connect(m_pipeline->bus(), "message", this, &QtGStreamerCaptureBackend::onBusMessage);
    m_pipeline->setState(QGst::StatePlaying);
}

void QtGStreamerCaptureBackend::stopCapture()
{
    if (m_pipeline) { // pipeline exists - destroy it
        // send an end-of-stream event to flush metadata and cause an EosMessage to be delivered
        m_pipeline->sendEvent(QGst::EosEvent::create());
    }
}

void QtGStreamerCaptureBackend::stopPipeline()
{
    if (!m_pipeline) {
        qCWarning(LIBSOUND_LOG) << "Stopping non-existing pipeline, aborting";
        return;
    }
    m_pipeline->setState(QGst::StateNull);
    m_pipeline.clear();
}

QStringList QtGStreamerCaptureBackend::devices() const
{
    // TODO qtgstreamer backend currently only provides access to default backend,
    // reenable selection by using Gst::Device

    return m_availableDevices.values();
}

void QtGStreamerCaptureBackend::setDevice(const QString &deviceIdentifier)
{
    // TODO add sanity check
    m_device = deviceIdentifier;
}
