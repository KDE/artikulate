/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef RECORDER_H
#define RECORDER_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QTemporaryFile>
#include <QUrl>

class ARTIKULATECORE_EXPORT Recorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString recordingFile READ recordingFile NOTIFY recordingFileChanged)
    Q_PROPERTY(CaptureState state READ state NOTIFY stateChanged)

public:
    Q_ENUMS(CaptureState)
    enum CaptureState { StoppedState = 0, RecordingState = 1 };

    explicit Recorder(QObject *parent = nullptr);
    ~Recorder() override;

    Q_INVOKABLE void startCapture();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void storeToFile(const QString &path);
    Q_INVOKABLE void clearBuffer();
    CaptureState state() const;
    QString recordingFile() const;

Q_SIGNALS:
    void stateChanged();
    void recordingFileChanged();

private:
    Q_DISABLE_COPY(Recorder)
    CaptureState m_state;
    QTemporaryFile m_recordingBufferFile;
};

#endif // RECORDER_H
