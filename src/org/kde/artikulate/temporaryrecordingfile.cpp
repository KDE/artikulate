// SPDX-FileCopyrightText: 2025 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "temporaryrecordingfile.h"
#include <QDir>

TemporaryRecordingFile::TemporaryRecordingFile(QObject *parent)
    : QObject{parent}
    , m_soundFile(new QTemporaryFile("XXXXXX.ogg"))
{
    iterate();
}

QUrl TemporaryRecordingFile::path() const
{
    if (!m_soundFile) {
        qCritical() << "Sound file was not created, return empty path";
        return QUrl();
    }
    const auto path = QUrl::fromLocalFile(QDir::tempPath() + '/' + m_soundFile->fileName());
    return path;
}

bool TemporaryRecordingFile::isSoundAvailable() const
{
    return m_soundAvailable;
}

void TemporaryRecordingFile::setSoundAvailable(bool available)
{
    if (m_soundAvailable == available) {
        return;
    }
    m_soundAvailable = available;
    Q_EMIT soundAvailableChanged();
}

void TemporaryRecordingFile::iterate()
{
    m_soundFile = std::make_unique<QTemporaryFile>("XXXXXX.ogg");
    m_soundFile->open();
    setSoundAvailable(false);
    Q_EMIT fileChanged();
}

void TemporaryRecordingFile::saveAs(const QString &path)
{
    if (path.isEmpty()) {
        qCritical() << "Target file is not a local file, skip saving:" << path;
        return;
    }
    if (!isSoundAvailable()) {
        qWarning() << "Temporary sound is not available, skipping";
        return;
    }
    if (QFile::exists(path)) {
        QFile::remove(path);
    }
    if (!QFile::copy(QDir::tempPath() + '/' + m_soundFile->fileName(), path)) {
        qWarning() << "Copy operation failed:" << QDir::tempPath() + '/' + m_soundFile->fileName() << "->" << path;
    }
}
