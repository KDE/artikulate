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
#include "core/skeleton.h"
#include "core/course.h"

#include <KLocale>
#include <QUuid>

NewCourseDialog::NewCourseDialog(ResourceManager *m_resourceManager)
    : KDialog(0)
    , m_resourceManager(m_resourceManager)
    , m_createdCourse(0)
{
    setPlainCaption(i18n("Create New Course"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);

    QWidget *widget = new QWidget(this);
    ui = new Ui::NewCourseDialog;

    ui->setupUi(widget);
    setMainWidget(widget);

    // add languages
    foreach (Language *language, m_resourceManager->languageList()) {
        ui->language->addItem(language->title(), language->id());
    }

    // add skeletons
    ui->skeletonSelector->addItem(i18n("< no skeleton >"), "");
    foreach (Skeleton *skeleton, m_resourceManager->skeletonList()) {
        ui->skeletonSelector->addItem(skeleton->title(), skeleton->id());
    }

    ui->title->setText(i18n("New Course"));

    connect(this, SIGNAL(okClicked()), this, SLOT(createCourse()));
}

NewCourseDialog::~NewCourseDialog()
{
    delete ui;
}

void NewCourseDialog::createCourse()
{
    Course *course = new Course(m_resourceManager);
    course->setId(QUuid::createUuid().toString());
    course->setTitle(ui->title->text());
    course->setDescription(ui->description->toHtml());

    // set skeleton
    QString skeletonId = ui->skeletonSelector->itemData(ui->skeletonSelector->currentIndex()).toString();
    course->setForeignId(skeletonId);

    // set language
    QString selectedLanguage = ui->language->itemData(ui->language->currentIndex()).toString();
    foreach (Language *language, m_resourceManager->languageList()) {
        if (language->id() == selectedLanguage) {
            course->setLanguage(language);
            break;
        }
    }
    // check that language actually exists
    Q_ASSERT(course->language());

    m_createdCourse = course;
}

Course * NewCourseDialog::course() const
{
    return m_createdCourse;
}
