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
    virtual ~QtMultimediaOutputBackend();

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
    QMediaPlayer *m_player;
};

#endif
