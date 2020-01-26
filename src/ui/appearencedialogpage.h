/*
 *  Copyright 2013  Magdalena Konkiewicz <konkiewicz.m@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef APPEARENCEDIALOGPAGE_H
#define APPEARENCEDIALOGPAGE_H

#include "ui_appearencedialogpage.h"
#include <QWidget>

class AppearenceDialogPage : public QWidget
{
    Q_OBJECT

public:
    AppearenceDialogPage();
    ~AppearenceDialogPage();

public slots:
    void saveSettings();
    void loadSettings();

private:
    Ui::AppearenceDialogPage *ui;
};

#endif
