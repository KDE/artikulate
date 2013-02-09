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

#include "newcoursedialog.h"
#include "core/resourcemanager.h"
#include "core/language.h"
#include <KLocale>

NewCourseDialog::NewCourseDialog(ResourceManager *resourceMgr)
    : KDialog(0)
    , resourceMgr(resourceMgr)
{
    setPlainCaption(i18n("Create New Course"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);

    QWidget *widget = new QWidget(this);
    ui = new Ui::NewCourseDialog;

    ui->setupUi(widget);
    setMainWidget(widget);

    // add languages
    foreach (Language *language, resourceMgr->languageList()) {
        ui->language->addItem(language->title(), language->id());
    }

    connect(this, SIGNAL(okClicked()), this, SLOT(createCourse()));
}

NewCourseDialog::~NewCourseDialog()
{
    delete ui;
}

void NewCourseDialog::createCourse()
{

}
