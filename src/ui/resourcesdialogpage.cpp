/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "resourcesdialogpage.h"
#include "core/resourcemanager.h"
#include "core/language.h"
#include "core/skeleton.h"
#include "core/course.h"
#include "settings.h"

#include <KLocalizedString>
#include <QLineEdit>
#include <QFileDialog>
#include <QToolButton>
#include <QUuid>

ResourcesDialogPage::ResourcesDialogPage(ResourceManager *m_resourceManager)
    : QWidget(nullptr)
    , m_resourceManager(m_resourceManager)
    , m_restartNeeded(false)
{
    ui = new Ui::ResourcesDialogPage;
    ui->setupUi(this);

    connect(ui->buttonSelectCourseRepository, &QToolButton::clicked, this, [=](){
        const QString dir = QFileDialog::getExistingDirectory(this,
            i18n("Open Repository Directory"),
            QString(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->kcfg_CourseRepositoryPath->setText(dir);
    });
}

ResourcesDialogPage::~ResourcesDialogPage()
{
    delete ui;
}

void ResourcesDialogPage::loadSettings()
{
    // setup Ui with stored settings
    ui->kcfg_CourseRepositoryPath->setText(Settings::courseRepositoryPath());
    ui->kcfg_UseCourseRepository->setChecked(Settings::useCourseRepository());
}

void ResourcesDialogPage::saveSettings()
{
    // save settings
    Settings::setUseCourseRepository(ui->kcfg_UseCourseRepository->isChecked());
    Settings::setCourseRepositoryPath(ui->kcfg_CourseRepositoryPath->text());
    Settings::self()->save();
    // reloading resources
    m_resourceManager->loadCourseResources();
}
