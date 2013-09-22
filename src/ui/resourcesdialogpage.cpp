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

#include "resourcesdialogpage.h"
#include "core/resourcemanager.h"
#include "core/language.h"
#include "core/skeleton.h"
#include "core/course.h"
#include "settings.h"

#include <KLocale>
#include <QUuid>

ResourcesDialogPage::ResourcesDialogPage(ResourceManager *m_resourceManager)
    : QWidget(0)
    , m_resourceManager(m_resourceManager)
    , m_restartNeeded(false)
{
    ui = new Ui::ResourcesDialogPage;
    ui->setupUi(this);

    // activate path selector only if repository should be used
    connect(ui->kcfg_UseCourseRepository, SIGNAL(toggled(bool)), this, SLOT(updateHints()));
    connect(ui->kcfg_CourseRepositoryPath, SIGNAL(textChanged(QString)), this, SLOT(updateHints()));

    updateHints();
}

ResourcesDialogPage::~ResourcesDialogPage()
{
    delete ui;
}

void ResourcesDialogPage::updateHints()
{
    if (ui->kcfg_UseCourseRepository->isChecked() != Settings::useCourseRepository()
        || ui->kcfg_UseCourseRepository->text() != Settings::courseRepositoryPath() )
    {
        m_restartNeeded = true;
    } else {
        m_restartNeeded = false;
    }
    // TODO
    // when doing this, a simple reload of the resource manager should be enough
    // proper reload mechanism not implemented yet
//     ui->restartInfo->setVisible(m_restartNeeded);//FIXME tell resource manager to reload
}

void ResourcesDialogPage::loadSettings()
{
    // setup Ui with stored settings
    ui->kcfg_CourseRepositoryPath->setText(Settings::courseRepositoryPath());
    ui->kcfg_UseCourseRepository->setChecked(Settings::useCourseRepository());
//     ui->restartInfo->setVisible(false); //FIXME tell resource manager
}

void ResourcesDialogPage::saveSettings()
{
    // save settings
    Settings::setUseCourseRepository(ui->kcfg_UseCourseRepository->isChecked());
    Settings::setCourseRepositoryPath(ui->kcfg_CourseRepositoryPath->text());
    Settings::self()->writeConfig();
}
