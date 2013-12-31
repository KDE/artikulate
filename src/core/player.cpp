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

#include "player.h"
#include "outputdevicecontroller.h"

#include <QList>
#include <KDebug>
#include <KUrl>
#include <QString>

Player::Player(QObject *parent)
    : QObject(parent)
    , m_soundFile(QString())
    , m_playbackState(StoppedState)
{

}

Player::~Player()
{
    // nothing to do
}

void Player::setSoundFile(const KUrl &fileUrl)
{
    if (!fileUrl.isValid() || fileUrl.isEmpty()) {
        kWarning() << "Not setting empty sound file path.";
        return;
    }
    m_soundFile = fileUrl;
    emit soundFileChanged();
}

void Player::setSoundFile(const QString& fileUrl)
{
    OutputDeviceController::self().stop();
    setSoundFile(KUrl::fromLocalFile(fileUrl));
}

QString Player::soundFile() const
{
    return m_soundFile.toLocalFile();
}

Player::PlaybackState Player::state() const
{
    return m_playbackState;
}

void Player::playback()
{
    if (m_soundFile.isEmpty()) {
        kError() << "Abort playing sound, no file available";
        return;
    }
    kDebug() << this << "Playback sound in file "<< m_soundFile.toLocalFile();
    OutputDeviceController::self().play(KUrl::fromLocalFile(m_soundFile.toLocalFile()));
    m_playbackState = PlayingState;
    connect(&OutputDeviceController::self(), SIGNAL(stopped()), this, SLOT(updateState()));
    emit stateChanged();
}

void Player::stop()
{
    OutputDeviceController::self().stop();
    OutputDeviceController::self().disconnect();
    emit stateChanged();
}

void Player::updateState()
{
    if (!OutputDeviceController::self().state() == Phonon::StoppedState) {
        return;
    }
    if (state() == PlayingState) {
        m_playbackState = StoppedState;
        emit stateChanged();
    }
    OutputDeviceController::self().disconnect();
}
