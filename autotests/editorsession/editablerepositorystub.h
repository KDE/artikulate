/*
 *  Copyright 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef EDITABLEREPOSITORYSTUB_H
#define EDITABLEREPOSITORYSTUB_H

#include "core/ieditablerepository.h"
#include "core/ieditablecourse.h"
#include <QObject>
#include <QVector>

class Language;

/**
 * @brief The EditableRepositoryStub is simple sub class only for testing
 */
class EditableRepositoryStub : public IEditableRepository
{
    Q_OBJECT
public:
    EditableRepositoryStub(QVector<IEditableCourse *> courses)
        : m_courses{ courses }
    {
    }
    ~EditableRepositoryStub() override;
    QString storageLocation() const override
    {
        return QString();
    }
    QVector<IEditableCourse *> editableCourses() const override
    {
        return m_courses;
    }
    QVector<ICourse *> courses() const override
    {
        QVector<ICourse *> courses;
        for (auto course : m_courses) {
            courses.push_back(course);
        }
        return courses;
    }
    QVector<ICourse *> courses(Language *language) const override
    {
        Q_UNUSED(language);
        return QVector<ICourse *>();
    }
    IEditableCourse * editableCourse(Language *language, int index) const override
    {
        Q_UNUSED(language);
        Q_UNUSED(index);
        return nullptr;
    }
    void reloadCourses() override
    {
        // do nothing
    }
    QVector<Language *> languages() const override
    {
        return QVector<Language *>();
    }
Q_SIGNALS:
    void courseAboutToBeAdded(ICourse*,int) override;
    void courseAdded() override;
    void courseAboutToBeRemoved(int) override;
    void courseRemoved() override;
private:
    QVector<IEditableCourse *> m_courses;
};

#endif
