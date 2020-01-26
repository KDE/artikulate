/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef EXPORTGHNSDIALOG_H
#define EXPORTGHNSDIALOG_H

#include "ui_exportghnsdialog.h"
#include <QDialog>
#include <QTemporaryFile>

class IResourceRepository;

class ExportGhnsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportGhnsDialog(IResourceRepository *repository);
    ~ExportGhnsDialog();

public Q_SLOTS:
    void onExportCourse();

private:
    Ui::ExportGhnsDialog *ui;
};

#endif
