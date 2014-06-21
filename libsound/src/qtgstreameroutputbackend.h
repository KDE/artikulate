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

#ifndef QTGSTREAMEROUTPUTBACKEND_H
#define QTGSTREAMEROUTPUTBACKEND_H

#include "capturedevicecontroller.h"
#include <QString>
#include <QTimer>
#include <QGst/Global>
#include <QGst/Pipeline>


class QtGStreamerOutputBackend : public QObject
{
    Q_OBJECT

public:
    QtGStreamerOutputBackend();
    ~QtGStreamerOutputBackend();

    void setUri(const QString & uri);

    QTime position() const;
    void setPosition(const QTime & pos);
    int volume() const;

    QTime length() const;
    QGst::State state() const;

public Q_SLOTS:
    void play();
    void pause();
    void stop();
    void setVolume(int volume);

Q_SIGNALS:
    void positionChanged();
    void stateChanged();

private:
    void onBusMessage(const QGst::MessagePtr &message);
    void handlePipelineStateChange(const QGst::StateChangedMessagePtr &scm);

    QGst::PipelinePtr m_pipeline;
    QTimer m_positionTimer;
};


#endif
