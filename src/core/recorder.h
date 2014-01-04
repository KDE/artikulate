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

#ifndef RECORDER_H
#define RECORDER_H

#include "artikulatecore_export.h"
#include <QObject>
#include <KUrl>

class ARTIKULATELIB_EXPORT Recorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString soundFileUrl READ soundFile WRITE setSoundFile NOTIFY soundFileChanged)
    Q_PROPERTY(CaptureState state READ state NOTIFY stateChanged)

public:
    Q_ENUMS(CaptureState)
    enum CaptureState {
        StoppedState = 0,
        RecordingState = 1
    };

    explicit Recorder(QObject *parent = 0);
    ~Recorder();

    Q_INVOKABLE void startCapture();
    Q_INVOKABLE void stop();
    CaptureState state() const;
    void setSoundFile(const KUrl &fileUrl);
    void setSoundFile(const QString &fileUrl);
    QString soundFile() const;

Q_SIGNALS:
    void stateChanged();
    void soundFileChanged();

private:
    Q_DISABLE_COPY(Recorder)
    KUrl m_soundFile;
    CaptureState m_state;
};

#endif // RECORDER_H
