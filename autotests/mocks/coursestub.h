/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef COURSESTUB_H
#define COURSESTUB_H

#include "src/core/icourse.h"
#include "src/core/language.h"
#include "src/core/unit.h"

#include <QObject>

class CourseStub : public ICourse
{
public:
    CourseStub(std::shared_ptr<Language> language, QVector<std::shared_ptr<Unit>> units)
        : m_language(language)
        , m_units(units)
    {
    }
    ~CourseStub() override;

    static std::shared_ptr<ICourse> create(std::shared_ptr<Language> language, QVector<std::shared_ptr<Unit>> units)
    {
        auto course = std::make_shared<CourseStub>(language, units);
        course->setSelf(course);
        return course;
    }

    void setSelf(std::shared_ptr<ICourse> self) override
    {
        m_self = self;
    }

    QString id() const override
    {
        return "courseid";
    }
    QString foreignId() const override
    {
        return "foreigncourseid";
    }
    QString title() const override
    {
        return m_title;
    }
    void setTitle(QString title)
    {
        m_title = title;
        emit titleChanged();
    }
    QString i18nTitle() const override
    {
        return "i18n title";
    }
    QString description() const override
    {
        return "description of the course";
    }
    std::shared_ptr<Language> language() const override
    {
        return m_language;
    }
    QVector<std::shared_ptr<Unit>> units() override
    {
        return m_units;
    }
    QUrl file() const override
    {
        return QUrl();
    }

private:
    QString m_title{ "title" };
    std::weak_ptr<ICourse> m_self;
    std::shared_ptr<Language> m_language;
    QVector<std::shared_ptr<Unit>> m_units;
};


#endif
