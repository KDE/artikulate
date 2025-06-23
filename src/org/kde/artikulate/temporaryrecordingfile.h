// SPDX-FileCopyrightText: 2025 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#ifndef TEMPORARYRECORDINGFILE_H
#define TEMPORARYRECORDINGFILE_H

#include <QObject>
#include <QQmlEngine>
#include <QTemporaryFile>
#include <memory>

class TemporaryRecordingFile : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QUrl path READ path NOTIFY fileChanged FINAL)
    Q_PROPERTY(bool soundAvailable READ isSoundAvailable WRITE setSoundAvailable NOTIFY soundAvailableChanged FINAL)

public:
    explicit TemporaryRecordingFile(QObject *parent = nullptr);
    QUrl path() const;
    bool isSoundAvailable() const;
    void setSoundAvailable(bool available);

public Q_SLOTS:
    /**
     * @brief clears the current file and creates a new one to be used
     */
    void iterate();
    /**
     * @brief save a copy of the temporary file at given location
     * @param path local file path
     */
    void saveAs(const QString &path);

Q_SIGNALS:
    void fileChanged();
    void soundAvailableChanged();

private:
    std::unique_ptr<QTemporaryFile> m_soundFile;
    bool m_soundAvailable{false};
};

#endif // TEMPORARYRECORDINGFILE_H
