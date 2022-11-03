/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef QTMULTIMEDIAOUTPUTBACKEND_H
#define QTMULTIMEDIAOUTPUTBACKEND_H

#include "outputbackendinterface.h"
#include <QString>

class QMediaPlayer;

class QtMultimediaOutputBackend : public OutputBackendInterface
{
    Q_OBJECT

public:
    explicit QtMultimediaOutputBackend(QObject *parent = nullptr);
    ~QtMultimediaOutputBackend() override;

    void setUri(const QString &uri) override;
    /**
     * volume as cubic value
     */
    int volume() const override;

    OutputDeviceController::State state() const override;

public Q_SLOTS:
    void play() override;
    void pause() override;
    void stop() override;
    void setVolume(int volume) override;

private:
    QMediaPlayer *m_player = nullptr;
};

#endif
