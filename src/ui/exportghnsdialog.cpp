/*
 *  Copyright 2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "exportghnsdialog.h"
#include "artikulate_debug.h"
#include "core/icourse.h"
#include "core/iresourcerepository.h"
#include "core/language.h"
#include "core/resources/courseparser.h"
#include "core/resources/editablecourseresource.h"
#include <KLocalizedString>
#include <QFileDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QWidget>

ExportGhnsDialog::ExportGhnsDialog(IResourceRepository *repository)
{
    ui = new Ui::ExportGhnsDialog;
    ui->setupUi(this);

    // require to set a proper directory
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText(i18n("Export"));
    connect(ui->exportDirectory, &QLineEdit::textChanged, this, [=]() {
        const bool directorySet = !ui->exportDirectory->text().isEmpty();
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(directorySet);
    });
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ExportGhnsDialog::onExportCourse);

    // directory selection dialog
    connect(ui->selectDirectoryButton, &QToolButton::clicked, this, [=]() {
        // TODO save last path in config file
        const QString dir = QFileDialog::getExistingDirectory(this, i18n("Export Directory"), QString(), QFileDialog::ShowDirsOnly);
        ui->exportDirectory->setText(dir);
    });

    // add courses to combo box
    int counter = 0;
    for (auto language : repository->languages()) {
        for (auto course : repository->courses(language->id())) {
            ui->courseListCombo->insertItem(counter, course->i18nTitle(), QVariant::fromValue<QObject *>(course.get()));
            ++counter;
        }
    }

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ExportGhnsDialog::~ExportGhnsDialog()
{
    delete ui;
}

void ExportGhnsDialog::onExportCourse()
{
    IEditableCourse *res = qobject_cast<IEditableCourse *>(ui->courseListCombo->currentData().value<QObject *>());
    qCDebug(ARTIKULATE_LOG) << res << "export GHNS file for" << res->i18nTitle();
    CourseParser::exportCourseToGhnsPackage(res->self(), ui->exportDirectory->text());
}
