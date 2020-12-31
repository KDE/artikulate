/*
    SPDX-FileCopyrightText: 2010 Marco Ballesio <gibrovacco@gmail.com>
    SPDX-FileCopyrightText: 2011 Collabora Ltd.
    SPDX-FileContributor: George Kiagiadakis <george.kiagiadakis@collabora.co.uk>
    SPDX-FileCopyrightText: 2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef QTGSTREAMEROUTPUTBACKEND_H
#define QTGSTREAMEROUTPUTBACKEND_H

#include "outputbackendinterface.h"
#include <QGst/Global>
#include <QGst/Pipeline>
#include <QString>
#include <QTimer>

class QtGStreamerOutputBackend : public OutputBackendInterface
{
    Q_OBJECT

public:
    QtGStreamerOutputBackend();
    virtual ~QtGStreamerOutputBackend();

    void setUri(const QString &uri);

    QTime position() const;
    void setPosition(const QTime &pos);
    /**
     * volume as cubic value
     */
    int volume() const;

    QTime length() const;
    OutputDeviceController::State state() const;

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
