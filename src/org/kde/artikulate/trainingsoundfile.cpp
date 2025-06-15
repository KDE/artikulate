// SPDX-FileCopyrightText: 2025 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "trainingsoundfile.h"
#include <QDir>

TrainingSoundFile::TrainingSoundFile(QObject *parent)
    : QObject{parent}
    , m_soundFile(new QTemporaryFile("XXXXXX.ogg"))
{
    iterate();
}

QUrl TrainingSoundFile::path() const
{
    if (!m_soundFile) {
        qCritical() << "Sound file was not created, return empty path";
        return QUrl();
    }
    const auto path = QUrl::fromLocalFile(QDir::tempPath() + '/' + m_soundFile->fileName());
    return path;
}

bool TrainingSoundFile::isSoundAvailable() const
{
    return m_soundAvailable;
}

void TrainingSoundFile::setSoundAvailable(bool available)
{
    if (m_soundAvailable == available) {
        return;
    }
    m_soundAvailable = available;
    Q_EMIT soundAvailableChanged();
}

void TrainingSoundFile::iterate()
{
    m_soundFile = std::make_unique<QTemporaryFile>("XXXXXX.ogg");
    m_soundFile->open();
    setSoundAvailable(false);
    Q_EMIT fileChanged();
}
