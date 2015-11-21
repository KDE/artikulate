/*
 *  Copyright 2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "exportghnsdialog.h"
#include "core/resourcemanager.h"
#include "core/resources/languageresource.h"
#include "core/resources/courseresource.h"
#include "artikulate_debug.h"
#include <KLocalizedString>
#include <QFileDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QWidget>

ExportGhnsDialog::ExportGhnsDialog(ResourceManager *manager)
    : m_manager(manager)
{
    ui = new Ui::ExportGhnsDialog;
    ui->setupUi(this);

    // require to set a proper directory
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText(i18n("Export"));
    connect(ui->exportDirectory, &QLineEdit::textChanged, this, [=](){
        const bool directorySet = !ui->exportDirectory->text().isEmpty();
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(directorySet);
    });
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
        this, &ExportGhnsDialog::onExportCourse);

    // directory selection dialog
    connect(ui->selectDirectoryButton, &QToolButton::clicked, this, [=]() {
        // TODO save last path in config file
        const QString dir = QFileDialog::getExistingDirectory(
            this,
            i18n("Export Directory"),
            QString(),
            QFileDialog::ShowDirsOnly);
        ui->exportDirectory->setText(dir);
    });

    // add courses to combo box
    int counter = 0;
    foreach (auto *languageRes, manager->languageResources()) {
        foreach (auto *courseRes, manager->courseResources(languageRes->language())) {
            ui->courseListCombo->insertItem(counter, courseRes->i18nTitle(),
                                            QVariant::fromValue<QObject*>(courseRes));
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
    CourseResource *res = qobject_cast<CourseResource *>(
        ui->courseListCombo->currentData().value<QObject*>());
    qCDebug(ARTIKULATE_LOG) << res << "export GHNS file for" << res->i18nTitle();
    res->exportGhns(ui->exportDirectory->text());
}
