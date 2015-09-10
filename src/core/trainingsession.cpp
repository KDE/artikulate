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

#include "trainingsession.h"
#include "core/language.h"
#include "core/course.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/phonemegroup.h"

TrainingSession::TrainingSession(QObject *parent)
    : QObject(parent)
    , m_language(nullptr)
    , m_course(nullptr)
    , m_unit(nullptr)
    , m_phrase(nullptr)
{

}

Language * TrainingSession::language() const
{
    return m_language;
}

void TrainingSession::setLanguage(Language *language)
{
    if (m_language == language) {
        return;
    }
    setCourse(nullptr);
    setUnit(nullptr);
    m_language = language;
    emit languageChanged();
}

Course * TrainingSession::course() const
{
    return m_course;
}

void TrainingSession::setCourse(Course *course)
{
    if (m_course == course) {
        return;
    }
    setUnit(nullptr);
    m_course = course;
    emit courseChanged();
}

Unit * TrainingSession::unit() const
{
    return m_unit;
}

void TrainingSession::setUnit(Unit *unit)
{
    if (m_unit == unit) {
        return;
    }
    m_unit = unit;
    return unitChanged();
}

Phrase * TrainingSession::phrase() const
{
    return m_phrase;
}

void TrainingSession::setPhrase(Phrase *phrase)
{
    if (m_phrase == phrase) {
        return;
    }
    setUnit(phrase->unit());
    m_phrase = phrase;
    return phraseChanged();
}