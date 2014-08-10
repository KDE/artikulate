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

#ifndef NEWCOURSEDIALOG_H
#define NEWCOURSEDIALOG_H

#include "ui_newcoursedialog.h"
#include <QDialog>

class ResourceManager;
class CourseResource;
class Language;

class NewCourseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewCourseDialog(ResourceManager *resourceMgr);
    virtual ~NewCourseDialog();
    void setLanguage(Language *language);
    CourseResource * courseResource() const;

public slots:
    void createCourse();

private:
    Ui::NewCourseDialog *ui;
    Language *m_fixedLanguage;
    ResourceManager *m_resourceManager;
    CourseResource *m_createdCourse;
};

#endif
