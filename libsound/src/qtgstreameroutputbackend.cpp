/*
 *  Copyright 2010  Marco Ballesio <gibrovacco@gmail.com>
 *  Copyright 2011  Collabora Ltd.
 *    @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>
 *  Copyright 2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "qtgstreameroutputbackend.h"
#include <QDir>
#include <QUrl>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/Bus>
#include <QGst/Init>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/ClockTime>
#include <QGst/Event>
#include <QGst/StreamVolume>

QtGStreamerOutputBackend::QtGStreamerOutputBackend()
{
    QGst::init();
}

QtGStreamerOutputBackend::~QtGStreamerOutputBackend()
{
    m_pipeline.clear();
}

void QtGStreamerOutputBackend::setUri(const QString & uri)
{
    QString realUri = uri;

    //if uri is not a real uri, assume it is a file path
    if (realUri.indexOf("://") < 0) {
        realUri = QUrl::fromLocalFile(realUri).toEncoded();
    }

    if (!m_pipeline) {
        m_pipeline = QGst::ElementFactory::make("playbin").dynamicCast<QGst::Pipeline>();
        if (m_pipeline) {
            //watch the bus for messages
            QGst::BusPtr bus = m_pipeline->bus();
            bus->addSignalWatch();
            QGlib::connect(bus, "message", this, &QtGStreamerOutputBackend::onBusMessage);
        } else {
            qCritical() << "Failed to create the pipeline";
        }
    }

    if (m_pipeline) {
        m_pipeline->setProperty("uri", realUri);
    }
}

QTime QtGStreamerOutputBackend::position() const
{
    if (m_pipeline) {
        //here we query the pipeline about its position
        //and we request that the result is returned in time format
        QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatTime);
        m_pipeline->query(query);
        return QGst::ClockTime(query->position()).toTime();
    } else {
        return QTime(0,0);
    }
}

void QtGStreamerOutputBackend::setPosition(const QTime & pos)
{
    QGst::SeekEventPtr evt = QGst::SeekEvent::create(
        1.0, QGst::FormatTime, QGst::SeekFlagFlush,
        QGst::SeekTypeSet, QGst::ClockTime::fromTime(pos),
        QGst::SeekTypeNone, QGst::ClockTime::None
    );

    m_pipeline->sendEvent(evt);
}

int QtGStreamerOutputBackend::volume() const
{
    if (m_pipeline) {
        QGst::StreamVolumePtr svp =
            m_pipeline.dynamicCast<QGst::StreamVolume>();

        if (svp) {
            return svp->volume(QGst::StreamVolumeFormatCubic) * 10;
        }
    }

    return 0;
}

void QtGStreamerOutputBackend::setVolume(int volume)
{
    if (m_pipeline) {
        QGst::StreamVolumePtr svp =
            m_pipeline.dynamicCast<QGst::StreamVolume>();

        if(svp) {
            svp->setVolume((double)volume / 10, QGst::StreamVolumeFormatCubic);
        }
    }
}

QTime QtGStreamerOutputBackend::length() const
{
    if (m_pipeline) {
        //here we query the pipeline about the content's duration
        //and we request that the result is returned in time format
        QGst::DurationQueryPtr query = QGst::DurationQuery::create(QGst::FormatTime);
        m_pipeline->query(query);
        return QGst::ClockTime(query->duration()).toTime();
    } else {
        return QTime(0,0);
    }
}

QGst::State QtGStreamerOutputBackend::state() const
{
    return m_pipeline ? m_pipeline->currentState() : QGst::StateNull;
}

void QtGStreamerOutputBackend::play()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StatePlaying);
    }
}

void QtGStreamerOutputBackend::pause()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StatePaused);
    }
}

void QtGStreamerOutputBackend::stop()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StateNull);

        //once the pipeline stops, the bus is flushed so we will
        //not receive any StateChangedMessage about this.
        //so, to inform the ui, we have to emit this signal manually.
        Q_EMIT stateChanged();
    }
    //TODO this is a temporary fix:
    // the pipeline should not be cleared after every stop, but only when the backend is destructed
    // or specifically resetted. Cause of the problem, both QtGStreamerOutputBackend is globally static
    // object and can be destructed _after_ the also globally static object QGlib::Private::ConnectionsStore
    // is destroyed. With calling QGst::Pipeline destructor by destructing QtGStreamerOutputBackend,
    // we get a crash.
    m_pipeline.clear();
}

void QtGStreamerOutputBackend::onBusMessage(const QGst::MessagePtr &message)
{
    switch (message->type()) {
    case QGst::MessageEos: //End of stream. We reached the end of the file.
        stop();
        break;
    case QGst::MessageError: //Some error occurred.
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
        stop();
        break;
    case QGst::MessageStateChanged: //The element in message->source() has changed state
        if (message->source() == m_pipeline) {
            handlePipelineStateChange(message.staticCast<QGst::StateChangedMessage>());
        }
        break;
    default:
        break;
    }
}

void QtGStreamerOutputBackend::handlePipelineStateChange(const QGst::StateChangedMessagePtr &scm)
{
    switch (scm->newState()) {
    case QGst::StatePlaying:
        //start the timer when the pipeline starts playing
        m_positionTimer.start(100);
        break;
    case QGst::StatePaused:
        //stop the timer when the pipeline pauses
        if(scm->oldState() == QGst::StatePlaying) {
            m_positionTimer.stop();
        }
        break;
    default:
        break;
    }

    Q_EMIT stateChanged();
}
