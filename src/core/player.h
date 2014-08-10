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

#ifndef PLAYER_H
#define PLAYER_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QUrl>

class ARTIKULATELIB_EXPORT Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString soundFileUrl READ soundFile WRITE setSoundFile NOTIFY soundFileChanged)
    Q_PROPERTY(PlaybackState state READ state NOTIFY stateChanged)

public:
    Q_ENUMS(PlaybackState)
    enum PlaybackState {
        StoppedState,
        PlayingState,
        PausedState
    };

    explicit Player(QObject *parent = 0);
    ~Player();

    Q_INVOKABLE void playback();
    Q_INVOKABLE void stop();
    PlaybackState state() const;
    void setSoundFile(const QUrl &fileUrl);
    void setSoundFile(const QString &fileUrl);
    QString soundFile() const;

Q_SIGNALS:
    void stateChanged();
    void soundFileChanged();

private Q_SLOTS:
    void updateState();

private:
    Q_DISABLE_COPY(Player)
    QUrl m_soundFile;
    PlaybackState m_playbackState;
};

#endif // PLAYER_H
