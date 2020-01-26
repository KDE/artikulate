/*
 *  Copyright 2013  Magdalena Konkiewicz <konkiewicz.m@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "appearencedialogpage.h"
#include "settings.h"

AppearenceDialogPage::AppearenceDialogPage()
    : QWidget(nullptr)
{
    ui = new Ui::AppearenceDialogPage;
    ui->setupUi(this);
}

AppearenceDialogPage::~AppearenceDialogPage()
{
    delete ui;
}

void AppearenceDialogPage::loadSettings()
{
    ui->kcfg_TrainingPhraseFont->setFont(Settings::trainingPhraseFont());
}

void AppearenceDialogPage::saveSettings()
{
    Settings::setTrainingPhraseFont(ui->kcfg_TrainingPhraseFont->font());
    Settings::self()->save();
}
