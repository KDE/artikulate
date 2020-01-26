/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef SOUNDDEVICEDIALOGPAGE_H
#define SOUNDDEVICEDIALOGPAGE_H

#include "ui_sounddevicedialogpage.h"
#include <QTemporaryFile>
#include <QWidget>

class SoundDeviceDialogPage : public QWidget
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
