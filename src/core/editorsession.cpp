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
#include "core/skeleton.h"
#include "core/language.h"
#include "core/course.h"
#include "core/resources/courseresource.h"
#include "core/resources/languageresource.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/resourcemanager.h"
#include <QDebug>

EditorSession::EditorSession(QObject *parent)
    : QObject(parent)
    , m_skeletonMode(true)
    , m_editSkeleton(false)
    , m_skeleton(nullptr)
    , m_language(nullptr)
    , m_course(nullptr)
    , m_tmpCourseWhileSkeletonEditing(nullptr)
    , m_unit(nullptr)
    , m_phrase(nullptr)
{

}

void EditorSession::setResourceManager(ResourceManager *manager)
{
    m_resourceManager = manager;
}

void EditorSession::setSkeletonMode(bool enabled)
{
    if (m_skeletonMode == enabled) {
        return;
    }
    m_skeletonMode = enabled;
    emit skeletonModeChanged();
}

bool EditorSession::skeletonMode() const
{
    return m_skeletonMode;
}

void EditorSession::setEditSkeleton(bool enabled)
{
    if (m_editSkeleton == enabled) {
        return;
    }
    m_editSkeleton = enabled;
    if (enabled) {
        m_tmpCourseWhileSkeletonEditing = m_course;
        setCourse(m_skeleton);
    } else {
        setCourse(m_tmpCourseWhileSkeletonEditing);
        m_tmpCourseWhileSkeletonEditing = nullptr;
    }
    emit editSkeletonChanged();
}

bool EditorSession::isEditSkeleton() const
{
    return m_editSkeleton;
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
    m_skeleton = skeleton;

    Language *language = m_language;
    if (!m_language) {
        language = m_resourceManager->languageResources().first()->language();
    }

    if (m_skeleton) {
        bool found = false;
        int resources = m_resourceManager->courseResources(language).count();
        for (int i=0; i < resources; ++i) {
            Course * course = m_resourceManager->course(language, i);
            if (course->foreignId() == m_skeleton->id()) {
                setCourse(course);
                found = true;
                break;
            }
        }
        if (!found) {
            setCourse(nullptr);
        }
    }

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
    m_language = language;
    if (m_skeletonMode) {
        bool found = false;
        if (m_skeleton) {
            int resources = m_resourceManager->courseResources(m_language).count();
            for (int i=0; i < resources; ++i) {
                Course * course = m_resourceManager->course(m_language, i);
                if (course->foreignId() == m_skeleton->id()) {
                    setCourse(course);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            setCourse(nullptr);
        }
    }
    else { // not skeleton mode
        if (m_resourceManager->courseResources(m_language).count() > 0) {
            setCourse(m_resourceManager->course(m_language, 0));
        }
    }
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
    m_course = course;
    if (m_course && !m_course->unitList().isEmpty()) {
        setUnit(m_course->unitList().first());
    } else {
        setUnit(nullptr);
    }
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
    // different than above, do not directly enter phrases
    // but first show editing information for units
    setPhrase(nullptr);
    return unitChanged();
}

void EditorSession::setPhrase(Phrase *phrase)
{
    if (m_phrase == phrase) {
        return;
    }
    if (phrase) {
        setUnit(phrase->unit());
    }
    m_phrase = phrase;
    return phraseChanged();
}

Phrase * EditorSession::phrase() const
{
    return m_phrase;
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
