/*
    SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QUrl>

class ARTIKULATECORE_EXPORT Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString soundFileUrl READ soundFile WRITE setSoundFile NOTIFY soundFileChanged)
    Q_PROPERTY(PlaybackState state READ state NOTIFY stateChanged)

public:
    Q_ENUMS(PlaybackState)
    enum PlaybackState { StoppedState, PlayingState, PausedState };

    explicit Player(QObject *parent = nullptr);
    ~Player() override = default;

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
