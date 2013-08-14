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
#include <core/capturedevicecontroller.h>
#include "settings.h"

#include <KLocale>
#include <KStandardDirs>
#include <QAudioDeviceInfo>
#include <QAudioCaptureSource>
#include <QMediaPlayer>

SoundDeviceDialogPage::SoundDeviceDialogPage()
    : QWidget(0)
    , m_audioOutput(new QMediaPlayer)
    , m_audioRecordedOutput(new QMediaPlayer)
{
    ui = new Ui::SoundDeviceDialogPage;
    ui->setupUi(this);

    // set buttons
    ui->buttonPlayTestSound->setIcon(KIcon("media-playback-start"));
    ui->buttonPlayRecordedTestSound->setIcon(KIcon("media-playback-start"));
    ui->buttonRecordTestSound->setIcon(KIcon("media-record"));
    ui->buttonPlayRecordedTestSound->setEnabled(false);

    // set input volume slider
    //TODO Qt5.x port: reenable, since that we can set audioinput volume
//     ui->kcfg_AudioInputVolume->setTickInterval(1);
//     ui->kcfg_AudioInputVolume->setMinimum(1);
//     ui->kcfg_AudioInputVolume->setMaximum(100);

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

    // temporary file for recording test
    m_recordTestFile.setSuffix(".ogg");
    m_recordTestFile.open();

    // connections
    connect(ui->buttonPlayTestSound, SIGNAL(clicked(bool)), this, SLOT(playTestSound()));
    connect(ui->buttonPlayRecordedTestSound, SIGNAL(clicked(bool)), this, SLOT(playRecordedSound()));
    connect(ui->buttonRecordTestSound, SIGNAL(clicked(bool)), this, SLOT(recordSound()));
    connect(m_audioOutput, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(updatePlayButtonIcons()));
    connect(m_audioRecordedOutput, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(updatePlayButtonIcons()));
}

SoundDeviceDialogPage::~SoundDeviceDialogPage()
{
    CaptureDeviceController::self().stopCapture();
    CaptureDeviceController::self().setDevice(Settings::audioInputDevice());

    delete ui;
}

void SoundDeviceDialogPage::loadSettings()
{
    ui->kcfg_AudioInputDevice->setCurrentIndex(
        ui->kcfg_AudioInputDevice->findText(Settings::audioInputDevice()));
//     ui->kcfg_AudioInputVolume->setValue(Settings::audioInputVolume());
    ui->kcfg_AudioOutputVolume->setValue(Settings::audioOutputVolume());
}

void SoundDeviceDialogPage::saveSettings()
{
    Settings::setAudioInputDevice(m_audioInputs.at(ui->kcfg_AudioInputDevice->currentIndex()));
//     Settings::setAudioInputVolume(ui->kcfg_AudioInputVolume->value());
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
    if (m_audioRecordedOutput->state() == QMediaPlayer::PlayingState) {
        m_audioRecordedOutput->stop();
        return;
    }
    m_audioRecordedOutput->setMedia(KUrl::fromLocalFile(m_recordTestFile.fileName()));
    m_audioRecordedOutput->setVolume(ui->kcfg_AudioOutputVolume->value());
    m_audioRecordedOutput->play();
}

void SoundDeviceDialogPage::recordSound()
{
    if (CaptureDeviceController::self().state() == CaptureDeviceController::RecordingState) {
        CaptureDeviceController::self().stopCapture();
        ui->buttonRecordTestSound->setIcon(KIcon("media-record"));
        ui->buttonPlayRecordedTestSound->setEnabled(true);
        return;
    }

    ui->buttonRecordTestSound->setIcon(KIcon("media-playback-stop"));
    CaptureDeviceController::self().setDevice(ui->kcfg_AudioInputDevice->currentText());
    CaptureDeviceController::self().startCapture(m_recordTestFile.fileName());
}

void SoundDeviceDialogPage::updatePlayButtonIcons()
{
    // default sound output test
    switch (m_audioOutput->state()) {
    case QMediaPlayer::PlayingState:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-stop"));
        break;
    case QMediaPlayer::StoppedState:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-start"));
        break;
    default:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-start"));
    }

    // recorded sound output test
    switch (m_audioRecordedOutput->state()) {
    case QMediaPlayer::PlayingState:
        ui->buttonPlayRecordedTestSound->setIcon(KIcon("media-playback-stop"));
        break;
    case QMediaPlayer::StoppedState:
        ui->buttonPlayRecordedTestSound->setIcon(KIcon("media-playback-start"));
        break;
    default:
        ui->buttonPlayRecordedTestSound->setIcon(KIcon("media-playback-start"));
    }
}

