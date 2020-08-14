/*
 *  SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "player.h"
#include "libsound/src/outputdevicecontroller.h"

#include "artikulate_debug.h"
#include <QList>
#include <QString>
#include <QUrl>

Player::Player(QObject *parent)
    : QObject(parent)
    , m_soundFile(QString())
    , m_playbackState(StoppedState)
{
}

void Player::setSoundFile(const QUrl &fileUrl)
{
    m_soundFile = fileUrl;
    emit soundFileChanged();
}

void Player::setSoundFile(const QString &fileUrl)
{
    OutputDeviceController::self().stop();
    setSoundFile(QUrl::fromLocalFile(fileUrl));
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
    OutputDeviceController::self().disconnect();
    if (!m_soundFile.isValid()) {
        qCritical() << "Abort playing sound, no file available";
        return;
    }
    qCDebug(ARTIKULATE_LOG) << this << "Playback sound in file " << m_soundFile.toLocalFile();
    OutputDeviceController::self().play(QUrl::fromLocalFile(m_soundFile.toLocalFile()));
    m_playbackState = PlayingState;
    connect(&OutputDeviceController::self(), &OutputDeviceController::started, this, &Player::updateState);
    connect(&OutputDeviceController::self(), &OutputDeviceController::stopped, this, &Player::updateState);
    emit stateChanged();
}

void Player::stop()
{
    OutputDeviceController::self().stop();
    OutputDeviceController::self().disconnect();
    m_playbackState = StoppedState;
    emit stateChanged();
}

void Player::updateState()
{
    if (OutputDeviceController::self().state() == OutputDeviceController::StoppedState && state() == PlayingState) {
        m_playbackState = StoppedState;
        emit stateChanged();
    }
    if (OutputDeviceController::self().state() == OutputDeviceController::PlayingState && state() != PlayingState) {
        m_playbackState = PlayingState;
        emit stateChanged();
    }
}
