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

#ifndef SOUNDDEVICEDIALOGPAGE_H
#define SOUNDDEVICEDIALOGPAGE_H

#include "ui_sounddevicedialogpage.h"
#include <QTemporaryFile>
#include <QWidget>


class SoundDeviceDialogPage :
    public QWidget
{
    Q_OBJECT

public:
    SoundDeviceDialogPage();
    ~SoundDeviceDialogPage();

public slots:
    void saveSettings();
    void loadSettings();
    void playTestSound();
    void playRecordedSound();
    void recordSound();
    void updatePlayButtonIcons();
    void stopPlaying();
    void stopRecord();
    void setVolume(int volume);

private:
    Ui::SoundDeviceDialogPage *ui;
    QList<QString> m_audioInputs;
    QList<QString> m_audioOutputs;
    QTemporaryFile m_recordTestFile;
};

#endif
