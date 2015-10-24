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

#include "editorsession.h"
#include "skeleton.h"
#include "core/language.h"
#include "core/course.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/phonemegroup.h"
#include <QDebug>

EditorSession::EditorSession(QObject *parent)
    : QObject(parent)
    , m_skeleton(nullptr)
    , m_language(nullptr)
    , m_course(nullptr)
    , m_unit(nullptr)
    , m_phrase(nullptr)
    , m_phonemeGroup(nullptr)
    , m_type(Phrase::Word)
{

}

Skeleton * EditorSession::skeleton() const
{
    return m_skeleton;
}

void EditorSession::setSkeleton(Skeleton *skeleton)
{
    if (m_skeleton == skeleton) {
        return;
    }
    // do not change language
    setCourse(nullptr);
    setUnit(nullptr);
    m_skeleton = skeleton;
    emit skeletonChanged();
}

Language * EditorSession::language() const
{
    return m_language;
}

void EditorSession::setLanguage(Language *language)
{
    if (m_language == language) {
        return;
    }
    setCourse(nullptr);
    setUnit(nullptr);
    m_language = language;
    emit languageChanged();
}

Course * EditorSession::course() const
{
    return m_course;
}

void EditorSession::setCourse(Course *course)
{
    if (m_course == course) {
        return;
    }
    setUnit(nullptr);
    setPhrase(nullptr);
    m_course = course;
    emit courseChanged();
}

Unit * EditorSession::unit() const
{
    return m_unit;
}

void EditorSession::setUnit(Unit *unit)
{
    if (m_unit == unit) {
        return;
    }
    m_unit = unit;
    setPhrase(nullptr);
    return unitChanged();
}

void EditorSession::setPhrase(Phrase *phrase)
{
    if (!phrase || m_phrase == phrase) {
        return;
    }
    setUnit(phrase->unit());
    m_phrase = phrase;
    return phraseChanged();
}

Phrase * EditorSession::phrase() const
{
    return m_phrase;
}

PhonemeGroup * EditorSession::phonemeGroup() const
{
    return m_phonemeGroup;
}

void EditorSession::setPhonemeGroup(PhonemeGroup* phonemeGroup)
{
    if (m_phonemeGroup == phonemeGroup) {
        return;
    }
    m_phonemeGroup = phonemeGroup;
    emit phonemeGroupChanged();
}

Phrase::Type EditorSession::phraseType() const
{
    return m_type;
}

void EditorSession::setPhraseType(Phrase::Type type)
{
    if (m_type == type) {
        return;
    }
    m_type = type;
    emit phraseTypeChanged(type);
}

Phrase * EditorSession::previousPhrase() const
{
    if (!m_phrase) {
        return nullptr;
    }
    const int index = m_phrase->unit()->phraseList().indexOf(m_phrase);
    if (index > 0) {
        return m_phrase->unit()->phraseList().at(index - 1);
    } else {
        Unit *unit = m_phrase->unit();
        int uIndex = unit->course()->unitList().indexOf(unit);
        if (uIndex > 0) {
            return unit->course()->unitList().at(uIndex - 1)->phraseList().last();
        }
    }
    return nullptr;
}

Phrase * EditorSession::nextPhrase() const
{
    if (!m_phrase) {
        return nullptr;
    }
    const int index = m_phrase->unit()->phraseList().indexOf(m_phrase);
    if (index < m_phrase->unit()->phraseList().length() - 1) {
        return m_phrase->unit()->phraseList().at(index + 1);
    } else {
        Unit *unit = m_phrase->unit();
        int uIndex = unit->course()->unitList().indexOf(unit);
        if (uIndex < unit->course()->unitList().length() - 1) {
            return unit->course()->unitList().at(uIndex + 1)->phraseList().first();
        }
    }
    return nullptr;
}

void EditorSession::switchToPreviousPhrase()
{
    setPhrase(previousPhrase());
}

void EditorSession::switchToNextPhrase()
{
    setPhrase(nextPhrase());
}

bool EditorSession::hasPreviousPhrase() const
{
    return previousPhrase() != nullptr;
}

bool EditorSession::hasNextPhrase() const
{
    return nextPhrase() != nullptr;
}
