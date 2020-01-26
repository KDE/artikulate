/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "sounddevicedialogpage.h"
#include "libsound/src/capturedevicecontroller.h"
#include "libsound/src/outputdevicecontroller.h"
#include "settings.h"

#include <KLocalizedString>
#include <QStandardPaths>

SoundDeviceDialogPage::SoundDeviceDialogPage()
    : QWidget(nullptr)
{
    ui = new Ui::SoundDeviceDialogPage;
    ui->setupUi(this);

    // set buttons
    ui->buttonPlayTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    ui->buttonPlayRecordedTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    ui->buttonRecordTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-record")));
    ui->buttonPlayRecordedTestSound->setEnabled(false);

    // set input volume slider
    // TODO Qt5.x port: reenable, since that we can set audioinput volume
    //     ui->kcfg_AudioInputVolume->setTickInterval(1);
    //     ui->kcfg_AudioInputVolume->setMinimum(1);
    //     ui->kcfg_AudioInputVolume->setMaximum(100);

    // set output volume slider
    ui->kcfg_AudioOutputVolume->setTickInterval(0);
    ui->kcfg_AudioOutputVolume->setMinimum(0);
    ui->kcfg_AudioOutputVolume->setMaximum(20);

    // devices
    QStringList devices = CaptureDeviceController::self().devices();
    for (int i = 0; i < devices.length(); ++i) {
        ui->kcfg_AudioInputDevice->insertItem(i, devices.at(i), i);
    }
    // TODO Gst::Device will allow selecting devices again with GStreamer 1.4

    // temporary file for recording test
    m_recordTestFile.open();

    // connections
    connect(ui->kcfg_AudioOutputVolume, &QAbstractSlider::valueChanged, this, &SoundDeviceDialogPage::setVolume);
    connect(ui->buttonPlayTestSound, &QAbstractButton::clicked, this, &SoundDeviceDialogPage::playTestSound);
    connect(ui->buttonPlayRecordedTestSound, &QAbstractButton::clicked, this, &SoundDeviceDialogPage::playRecordedSound);
    connect(ui->buttonRecordTestSound, &QAbstractButton::clicked, this, &SoundDeviceDialogPage::recordSound);
    connect(&OutputDeviceController::self(), &OutputDeviceController::started, this, &SoundDeviceDialogPage::updatePlayButtonIcons);
    connect(&OutputDeviceController::self(), &OutputDeviceController::stopped, this, &SoundDeviceDialogPage::updatePlayButtonIcons);
}

SoundDeviceDialogPage::~SoundDeviceDialogPage()
{
    CaptureDeviceController::self().stopCapture();
    CaptureDeviceController::self().setDevice(Settings::audioInputDevice());

    delete ui;
}

void SoundDeviceDialogPage::loadSettings()
{
    ui->kcfg_AudioInputDevice->setCurrentIndex(ui->kcfg_AudioInputDevice->findText(Settings::audioInputDevice()));
    //     ui->kcfg_AudioInputVolume->setValue(Settings::audioInputVolume());
    ui->kcfg_AudioOutputVolume->setValue(Settings::audioOutputVolume());
}

void SoundDeviceDialogPage::setVolume(int volume)
{
    OutputDeviceController::self().setVolume(volume);
}

void SoundDeviceDialogPage::saveSettings()
{
    Settings::setAudioInputDevice(ui->kcfg_AudioInputDevice->itemText(ui->kcfg_AudioInputDevice->currentIndex()));
    //     Settings::setAudioInputVolume(ui->kcfg_AudioInputVolume->value());
    Settings::setAudioOutputVolume(static_cast<int>(ui->kcfg_AudioOutputVolume->value()));
    OutputDeviceController::self().setVolume(ui->kcfg_AudioOutputVolume->value());
    Settings::self()->save();
}

void SoundDeviceDialogPage::playTestSound()
{
    if (OutputDeviceController::self().state() == OutputDeviceController::PlayingState) {
        OutputDeviceController::self().stop();
        return;
    }
    QString testsoundFile = QStandardPaths::locate(QStandardPaths::DataLocation, QStringLiteral("sounds/testsound.ogg"));
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

void SoundDeviceDialogPage::stopPlaying()
{
    OutputDeviceController::self().stop();
}

void SoundDeviceDialogPage::recordSound()
{
    if (CaptureDeviceController::self().state() == CaptureDeviceController::RecordingState) {
        CaptureDeviceController::self().stopCapture();
        ui->buttonRecordTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-record")));
        ui->buttonPlayRecordedTestSound->setEnabled(true);
        return;
    }

    ui->buttonRecordTestSound->setIcon(QIcon::fromTheme(QStringLiteral("artikulate-media-record-active")));
    CaptureDeviceController::self().setDevice(ui->kcfg_AudioInputDevice->currentText());
    CaptureDeviceController::self().startCapture(m_recordTestFile.fileName());
}

void SoundDeviceDialogPage::stopRecord()
{
    if (CaptureDeviceController::self().state() == CaptureDeviceController::RecordingState) {
        CaptureDeviceController::self().stopCapture();
        ui->buttonRecordTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-record")));
        ui->buttonPlayRecordedTestSound->setEnabled(true);
    }
}

void SoundDeviceDialogPage::updatePlayButtonIcons()
{
    // default sound output test
    switch (OutputDeviceController::self().state()) {
        case OutputDeviceController::PlayingState:
            ui->buttonPlayTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-stop")));
            break;
        case OutputDeviceController::StoppedState:
            ui->buttonPlayTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
            break;
        default:
            ui->buttonPlayTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    }

    // recorded sound output test
    switch (OutputDeviceController::self().state()) {
        case OutputDeviceController::PlayingState:
            ui->buttonPlayRecordedTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-stop")));
            break;
        case OutputDeviceController::StoppedState:
            ui->buttonPlayRecordedTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
            break;
        default:
            ui->buttonPlayRecordedTestSound->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    }
}
