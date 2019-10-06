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

#include "qtmultimediaoutputbackend.h"
#include <QDir>
#include <QMediaPlayer>
#include <QUrl>

QtMultimediaOutputBackend::QtMultimediaOutputBackend(QObject *parent)
    : OutputBackendInterface(parent)
    , m_player(new QMediaPlayer)
{
    connect(m_player, &QMediaPlayer::stateChanged, this, &QtMultimediaOutputBackend::stateChanged);
}

QtMultimediaOutputBackend::~QtMultimediaOutputBackend()
{
    m_player->deleteLater();
}

void QtMultimediaOutputBackend::setUri(const QString &uri)
{
    m_player->setMedia(QUrl::fromLocalFile(uri));
}

int QtMultimediaOutputBackend::volume() const
{
    return m_player->volume(); // TODO check if this is really a cubic value
}

void QtMultimediaOutputBackend::setVolume(int volume)
{
    m_player->setVolume(volume); // TODO check if value is cubic
}

OutputDeviceController::State QtMultimediaOutputBackend::state() const
{
    switch (m_player->state()) {
        case QMediaPlayer::StoppedState:
            return OutputDeviceController::StoppedState;
        case QMediaPlayer::PlayingState:
            return OutputDeviceController::PlayingState;
        case QMediaPlayer::PausedState:
            return OutputDeviceController::PausedState;
    }
    Q_UNREACHABLE();
    return OutputDeviceController::StoppedState;
}

void QtMultimediaOutputBackend::play()
{
    m_player->play();
}

void QtMultimediaOutputBackend::pause()
{
    m_player->pause();
}

void QtMultimediaOutputBackend::stop()
{
    m_player->stop();
}
