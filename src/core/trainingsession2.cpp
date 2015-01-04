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

#include "trainingsession2.h"
#include "core/language.h"
#include "core/course.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/phonemegroup.h"

TrainingSession2::TrainingSession2(QObject *parent)
    : QObject(parent)
    , m_language(0)
    , m_course(0)
    , m_unit(0)
{

}

Language * TrainingSession2::language() const
{
    return m_language;
}

void TrainingSession2::setLanguage(Language *language)
{
    if (m_language == language) {
        return;
    }
    setCourse(0);
    setUnit(0);
    m_language = language;
    emit languageChanged();
}

Course * TrainingSession2::course() const
{
    return m_course;
}

void TrainingSession2::setCourse(Course *course)
{
    if (m_course == course) {
        return;
    }
    setUnit(0);
    m_course = course;
    emit courseChanged();
}

Unit * TrainingSession2::unit() const
{
    return m_unit;
}

void TrainingSession2::setUnit(Unit *unit)
{
    if (m_unit == unit) {
        return;
    }
    m_unit = unit;
    return unitChanged();
}
