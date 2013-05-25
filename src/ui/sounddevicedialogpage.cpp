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

#include "sounddevicedialogpage.h"
#include "settings.h"

#include <KLocale>
#include <KStandardDirs>
#include <QAudioDeviceInfo>
#include <QAudioCaptureSource>
#include <QMediaPlayer>

SoundDeviceDialogPage::SoundDeviceDialogPage()
    : QWidget(0)
    , m_audioOutput(new QMediaPlayer)
{
    ui = new Ui::SoundDeviceDialogPage;
    ui->setupUi(this);

    // set buttons
    ui->buttonPlayTestSound->setIcon(KIcon("media-playback-start"));
    ui->buttonPlayRecordedTestSound->setIcon(KIcon("media-playback-start"));
    ui->buttonRecordTestSound->setIcon(KIcon("media-record"));

    // set input volume slider
    ui->kcfg_AudioInputVolume->setTickInterval(1);
    ui->kcfg_AudioInputVolume->setMinimum(1);
    ui->kcfg_AudioInputVolume->setMaximum(100);

    // set output volume slider
    ui->kcfg_AudioOutputVolume->setTickInterval(1);
    ui->kcfg_AudioOutputVolume->setMinimum(1);
    ui->kcfg_AudioOutputVolume->setMaximum(100);

    // devices
    QAudioCaptureSource captureSource; // = new QAudioCaptureSource(m_parent);
    m_audioInputs = captureSource.audioInputs();
    for (int i=0; i < m_audioInputs.length(); ++i) {
        ui->kcfg_AudioInputDevice->insertItem(i, m_audioInputs.at(i), i);
    }

    // connections
    connect(ui->buttonPlayTestSound, SIGNAL(clicked(bool)), this, SLOT(playTestSound()));
    connect(m_audioOutput, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(updateTestButtonIcons()));
}

SoundDeviceDialogPage::~SoundDeviceDialogPage()
{
    delete ui;
}

void SoundDeviceDialogPage::loadSettings()
{
//     ui->kcfg_AudioInputDevice(Settings::audioInputDevice());
    ui->kcfg_AudioInputVolume->setValue(Settings::audioInputVolume());
    ui->kcfg_AudioOutputVolume->setValue(Settings::audioOutputVolume());
}

void SoundDeviceDialogPage::saveSettings()
{
    Settings::setAudioInputDevice(m_audioInputs.at(ui->kcfg_AudioInputDevice->currentIndex()));
    Settings::setAudioInputVolume(ui->kcfg_AudioInputVolume->value());
    Settings::setAudioOutputVolume(ui->kcfg_AudioOutputVolume->value());
    Settings::self()->writeConfig();
}

void SoundDeviceDialogPage::playTestSound()
{
    if (m_audioOutput->state() == QMediaPlayer::PlayingState) {
        m_audioOutput->stop();
        return;
    }
    QString testsoundFile = KGlobal::dirs()->findResource("appdata", QString("sounds/testsound.ogg"));
    m_audioOutput->setMedia(KUrl::fromLocalFile(testsoundFile));
    m_audioOutput->setVolume(ui->kcfg_AudioOutputVolume->value());
    m_audioOutput->play();
}

void SoundDeviceDialogPage::playRecordedSound()
{
    //TODO
}

void SoundDeviceDialogPage::recordSound()
{
    //TODO
}

void SoundDeviceDialogPage::updateTestButtonIcons()
{
    switch (m_audioOutput->state()) {
    case QMediaPlayer::PlayingState:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-stop"));
        break;
    case QMediaPlayer::QMediaPlayer::StoppedState:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-start"));
        break;
    default:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-start"));
    }
}


