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

#ifndef PROFILE_H
#define PROFILE_H

#include "artikulatecore_export.h"
#include "course.h"
#include <QObject>
#include <QList>
#include <KUrl>

class QString;
class Language;
class Course;
class Unit;
class TagGroup;

/**
 * \class Profile
 * Objects of this class describe the current set values for language, course, unit, etc.
 * by a user.
 */
class ARTIKULATELIB_EXPORT Profile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Language* language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(Course* course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(Unit* unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(TagGroup* tagGroup READ tagGroup WRITE setTagGroup NOTIFY tagGroupChanged)

public:
    explicit Profile(QObject *parent = 0);

    Language * language() const;
    void setLanguage(Language *language);
    Course * course() const;
    void setCourse(Course *course);
    Unit * unit() const;
    void setUnit(Unit *unit);
    TagGroup * tagGroup() const;
    void setTagGroup(TagGroup *tagGroup);

signals:
    void languageChanged();
    void courseChanged();
    void unitChanged();
    void tagGroupChanged();

private:
    Q_DISABLE_COPY(Profile)
    Language *m_language;
    Course *m_course;
    Unit *m_unit;
    TagGroup *m_tagGroup;
};

#endif // PROFILE_H
