/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#include "resourcesdialog.h"
#include "core/resourcemanager.h"
#include "core/language.h"
#include "core/skeleton.h"
#include "core/course.h"
#include "settings.h"

#include <KLocale>
#include <QUuid>

ResourcesDialog::ResourcesDialog(ResourceManager *m_resourceManager)
    : KDialog(0)
    , m_resourceManager(m_resourceManager)
{
    setPlainCaption(i18n("Course Resources"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);

    QWidget *widget = new QWidget(this);
    ui = new Ui::ResourcesDialog;

    ui->setupUi(widget);
    setMainWidget(widget);

    // setup Ui with stored settings
    ui->repositoryUrl->setPath(Settings::courseRepositoryPath());
    if (Settings::useCourseRepository()) {
        ui->radioRepository->setChecked(true);
    } else {
        ui->radioRepository->setChecked(false);
    }
    ui->repositoryUrl->setEnabled(ui->radioRepository->isChecked());

    // activate path selector only if repository should be used
    connect(ui->radioRepository, SIGNAL(toggled(bool)), ui->repositoryUrl, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(okClicked()), this, SLOT(saveSettings()));
}

ResourcesDialog::~ResourcesDialog()
{
    delete ui;
}

void ResourcesDialog::saveSettings()
{
    // save settings
    Settings::setUseCourseRepository(ui->radioRepository->isChecked());
    Settings::setCourseRepositoryPath(ui->repositoryUrl->text());

    // set resource manager
    //TODO
}
