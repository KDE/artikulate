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

#include "newcoursedialog.h"
#include "core/resourcemanager.h"
#include "core/language.h"
#include "core/skeleton.h"
#include "core/course.h"
#include "core/resources/languageresource.h"
#include "core/resources/courseresource.h"
#include "core/resources/skeletonresource.h"
#include "settings.h"
#include <KLocalizedString>
#include <KStandardGuiItem>
#include <QPushButton>
#include <QUuid>
#include <QDialogButtonBox>

NewCourseDialog::NewCourseDialog(ResourceManager *m_resourceManager)
    : QDialog(0)
    , m_fixedLanguage(0)
    , m_resourceManager(m_resourceManager)
    , m_createdCourse(0)
{
    setWindowTitle(i18n("Create New Course"));

    QWidget *widget = new QWidget(this);
    ui = new Ui::NewCourseDialog;

    ui->setupUi(widget);

    QDialogButtonBox *buttons = new QDialogButtonBox(this);
    QPushButton *okButton = new QPushButton;
    KGuiItem::assign(okButton, KStandardGuiItem::ok());
    okButton->setShortcut(Qt::Key_Return);
    buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
    widget->layout()->addWidget(buttons);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    QPushButton *cancelButton = new QPushButton;
    KGuiItem::assign(cancelButton, KStandardGuiItem::cancel());
    okButton->setShortcut(Qt::Key_Cancel);
    buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

    //FIXME only allow creation when data repository is set
    //FIXME only allow creation when unique ID is given: check this and give feedback

    // add languages
    foreach (LanguageResource *resource, m_resourceManager->languageResources()) {
        ui->language->addItem(resource->title(), resource->identifier());
    }

    // add skeletons
    ui->skeletonSelector->addItem(i18n("none"), "");
    foreach (SkeletonResource *resource, m_resourceManager->skeletonResources()) {
        ui->skeletonSelector->addItem(resource->skeleton()->title(), resource->identifier());
    }

    ui->title->setText(i18n("New Course"));

    connect(this, SIGNAL(okClicked()), this, SLOT(createCourse()));
}

NewCourseDialog::~NewCourseDialog()
{
    delete ui;
}

void NewCourseDialog::setLanguage(Language *language)
{
    ui->language->setVisible(false);
    ui->labelLanguage->setVisible(false);
    m_fixedLanguage = language;
}

void NewCourseDialog::createCourse()
{
    // set language from dialog if not set yet
    Language * language = m_fixedLanguage;
    if (!language) {
        QString selectedLanguage = ui->language->itemData(ui->language->currentIndex()).toString();
        foreach (LanguageResource *resource, m_resourceManager->languageResources()) {
            if (resource->identifier() == selectedLanguage) {
                language = resource->language();
                break;
            }
        }
    }

    // set path
    QString path = QString("%1/%2/%3/%4/%4.xml")
        .arg(Settings::courseRepositoryPath())
        .arg("courses")
        .arg(ui->identifier->text())
        .arg(language->id());

    m_createdCourse = new CourseResource(m_resourceManager, QUrl::fromLocalFile(path));
    Q_ASSERT(m_createdCourse);

    Course *course = m_createdCourse->course();
    Q_ASSERT(course);
    course->setId(QUuid::createUuid().toString());
    course->setTitle(ui->title->text());
    course->setDescription(ui->description->toHtml());
    course->setFile(QUrl::fromLocalFile(path));
    course->setLanguage(language);

    // set skeleton
    QString skeletonId = ui->skeletonSelector->itemData(ui->skeletonSelector->currentIndex()).toString();
    course->setForeignId(skeletonId);
}

CourseResource * NewCourseDialog::courseResource() const
{
    return m_createdCourse;
}
