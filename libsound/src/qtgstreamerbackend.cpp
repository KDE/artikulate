/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "qtgstreamerbackend.h"

#include <QGlib/Error>
#include <QGlib/Connect>
#include <QGst/Init>
#include <QGst/ElementFactory>
#include <QGst/ChildProxy>
#include <QGst/Pipeline>
#include <QGst/Pad>
#include <QGst/Event>
#include <QGst/Message>
#include <QGst/Bus>

#include <KDebug>
#include <KLocale>

QtGStreamerBackend::QtGStreamerBackend()
{
    QGst::init();

    //setup the device list
    QGst::ElementPtr src = QGst::ElementFactory::make("autoaudiosrc");

    if (!src) {
        kError() << "Failed to create element \"autoaudiosrc\". Make sure you have "
                 << "gstreamer-plugins-good installed";
        return;
    }

    m_availableDevices.insert("", i18nc("default sound device", "Default"));
}

QtGStreamerBackend::~QtGStreamerBackend()
{
    m_pipeline.clear();
}

QString QtGStreamerBackend::identifier()
{
    return QLatin1String("qtgstreamerbackend");
}

CaptureDeviceController::State QtGStreamerBackend::captureState()
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

QGst::BinPtr QtGStreamerBackend::createAudioSrcBin()
{
    QGst::BinPtr audioBin;

    try {
        audioBin = QGst::Bin::fromDescription("autoaudiosrc name=\"audiosrc\" ! audioconvert ! "
                                              "audioresample ! audiorate ! vorbisenc name=enc quality=0.6 ! queue");
    } catch (const QGlib::Error &error) {
        kError() << "Failed to create audio source bin:" << error;
        return QGst::BinPtr();
    }
    QGst::ElementPtr src = audioBin->getElementByName("audiosrc");
    //autoaudiosrc creates the actual source in the READY state

    src->setState(QGst::StateReady);
    return audioBin;
}

void QtGStreamerBackend::onBusMessage(const QGst::MessagePtr & message)
{
    switch (message->type()) {
    case QGst::MessageEos:
        //got end-of-stream - stop the pipeline
        kDebug() << "EOS signal received, stopping pipeline";
        stopPipeline();
        break;
    case QGst::MessageError:
        //check if the pipeline exists before destroying it,
        //since we could get multiple error messages
        if (m_pipeline) {
            stopPipeline();
        }
        kError() << "Pipeline Error:"
                 << message.staticCast<QGst::ErrorMessage>()->error().message();
        break;
    default:
        break;
    }
}

void QtGStreamerBackend::startCapture(const QString &filePath)
{
    // clear pipeline if still existing
    if (m_pipeline) {
        kWarning() << "removing forgotten pipeline";
        //send an end-of-stream event to flush metadata and cause an EosMessage to be delivered
        m_pipeline->sendEvent(QGst::EosEvent::create());
    }

    QGst::BinPtr audioSrcBin = createAudioSrcBin();
    QGst::ElementPtr mux = QGst::ElementFactory::make("oggmux");
    QGst::ElementPtr sink = QGst::ElementFactory::make("filesink");

    if (!audioSrcBin || !mux || !sink) {
        kError() << "One or more elements could not be created. "
                 << "Verify that you have all the necessary element plugins installed.";
        return;
    }

    // set output path
    sink->setProperty("location", filePath);

    m_pipeline = QGst::Pipeline::create();
    m_pipeline->add(audioSrcBin, mux, sink);

    //link elements
    QGst::PadPtr audioPad = mux->getRequestPad("audio_%u");
    audioSrcBin->getStaticPad("src")->link(audioPad);

    mux->link(sink);

    //connect the bus
    m_pipeline->bus()->addSignalWatch();
    QGlib::connect(m_pipeline->bus(), "message", this, &QtGStreamerBackend::onBusMessage);
    m_pipeline->setState(QGst::StatePlaying);
}

void QtGStreamerBackend::stopCapture()
{
    if (m_pipeline) { //pipeline exists - destroy it
        //send an end-of-stream event to flush metadata and cause an EosMessage to be delivered
        m_pipeline->sendEvent(QGst::EosEvent::create());
    }
}

void QtGStreamerBackend::stopPipeline()
{
    if (!m_pipeline) {
        kWarning() << "Stopping non-existing pipeline, aborting";
        return;
    }
    m_pipeline->setState(QGst::StateNull);
    m_pipeline.clear();
}

QStringList QtGStreamerBackend::devices() const
{
    //TODO qtgstreamer backend currently only provides access to default backend,
    // reenable selection by using Gst::Device

    return m_availableDevices.keys();
}

void QtGStreamerBackend::setDevice(const QString& deviceIdentifier)
{
    //TODO add sanity check
    m_device = deviceIdentifier;
}
