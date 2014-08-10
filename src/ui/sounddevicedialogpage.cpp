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
#include "libsound/src/capturedevicecontroller.h"
#include "libsound/src/outputdevicecontroller.h"
#include "settings.h"

#include <KLocale>
#include <KStandardDirs>

SoundDeviceDialogPage::SoundDeviceDialogPage()
    : QWidget(0)
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
    ui->kcfg_AudioOutputVolume->setTickInterval(0);
    ui->kcfg_AudioOutputVolume->setMinimum(0);
    ui->kcfg_AudioOutputVolume->setMaximum(20);

    // devices
    QStringList devices = CaptureDeviceController::self().devices();
    for (int i=0; i < devices.length(); ++i) {
        ui->kcfg_AudioInputDevice->insertItem(i, devices.at(i), i);
    }
    //TODO Gst::Device will allow selecting devices again with GStreamer 1.4

    // temporary file for recording test
    m_recordTestFile.setSuffix(".ogg");
    m_recordTestFile.open();

    // connections
    connect(ui->buttonPlayTestSound, SIGNAL(clicked(bool)), this, SLOT(playTestSound()));
    connect(ui->buttonPlayRecordedTestSound, SIGNAL(clicked(bool)), this, SLOT(playRecordedSound()));
    connect(ui->buttonRecordTestSound, SIGNAL(clicked(bool)), this, SLOT(recordSound()));
    connect(&OutputDeviceController::self(), SIGNAL(started()), this, SLOT(updatePlayButtonIcons()));
    connect(&OutputDeviceController::self(), SIGNAL(stopped()), this, SLOT(updatePlayButtonIcons()));
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
    Settings::setAudioInputDevice(ui->kcfg_AudioInputDevice->itemText(ui->kcfg_AudioInputDevice->currentIndex()));
//     Settings::setAudioInputVolume(ui->kcfg_AudioInputVolume->value());
    Settings::setAudioOutputVolume((int) ui->kcfg_AudioOutputVolume->value());
    OutputDeviceController::self().setVolume(ui->kcfg_AudioOutputVolume->value());
    Settings::self()->writeConfig();
}

void SoundDeviceDialogPage::playTestSound()
{
    if (OutputDeviceController::self().state() == OutputDeviceController::PlayingState) {
        OutputDeviceController::self().stop();
        return;
    }
    QString testsoundFile = KGlobal::dirs()->findResource("appdata", QString("sounds/testsound.ogg"));
    OutputDeviceController::self().setVolume(ui->kcfg_AudioOutputVolume->value());
    OutputDeviceController::self().play(QUrl::fromLocalFile(testsoundFile));
}

void SoundDeviceDialogPage::playRecordedSound()
{
    if (OutputDeviceController::self().state() == OutputDeviceController::PlayingState) {
        OutputDeviceController::self().stop();
        return;
    }
    OutputDeviceController::self().setVolume(ui->kcfg_AudioOutputVolume->value());
    OutputDeviceController::self().play(QUrl::fromLocalFile(m_recordTestFile.fileName()));
}

void SoundDeviceDialogPage::recordSound()
{
    if (CaptureDeviceController::self().state() == CaptureDeviceController::RecordingState)
    {
        CaptureDeviceController::self().stopCapture();
        ui->buttonRecordTestSound->setIcon(KIcon("media-record"));
        ui->buttonPlayRecordedTestSound->setEnabled(true);
        return;
    }

    ui->buttonRecordTestSound->setIcon(KIcon("artikulate-media-record-active"));
    CaptureDeviceController::self().setDevice(ui->kcfg_AudioInputDevice->currentText());
    CaptureDeviceController::self().startCapture(m_recordTestFile.fileName());
}

void SoundDeviceDialogPage::updatePlayButtonIcons()
{
    // default sound output test
    switch (OutputDeviceController::self().state()) {
    case OutputDeviceController::PlayingState:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-stop"));
        break;
    case OutputDeviceController::StoppedState:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-start"));
        break;
    default:
        ui->buttonPlayTestSound->setIcon(KIcon("media-playback-start"));
    }

    // recorded sound output test
    switch (OutputDeviceController::self().state()) {
    case OutputDeviceController::PlayingState:
        ui->buttonPlayRecordedTestSound->setIcon(KIcon("media-playback-stop"));
        break;
    case OutputDeviceController::StoppedState:
        ui->buttonPlayRecordedTestSound->setIcon(KIcon("media-playback-start"));
        break;
    default:
        ui->buttonPlayRecordedTestSound->setIcon(KIcon("media-playback-start"));
    }
}
