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
#include "core/language.h"
#include "core/resources/editablecourseresource.h"
#include "core/resources/skeletonresource.h"
#include "core/unit.h"
#include "core/iunit.h"
#include "core/phrase.h"
#include "core/contributorrepository.h"
#include "artikulate_debug.h"

EditorSession::EditorSession(QObject *parent)
    : QObject(parent)
{
    connect(this, &EditorSession::skeletonChanged, this, &EditorSession::displayedCourseChanged);
    connect(this, &EditorSession::courseChanged, this, &EditorSession::displayedCourseChanged);
    connect(this, &EditorSession::editSkeletonChanged, this, &EditorSession::displayedCourseChanged);
    connect(this, &EditorSession::displayedCourseChanged, this, &EditorSession::updateDisplayedUnit);
    connect(this, &EditorSession::courseChanged, this, &EditorSession::skeletonModeChanged);
}

void EditorSession::setRepository(IEditableRepository *repository)
{
    m_repository = repository;
}

bool EditorSession::skeletonMode() const
{
    return m_skeleton != nullptr;
}

void EditorSession::setEditSkeleton(bool enabled)
{
    if (m_editSkeleton == enabled) {
        return;
    }
    m_editSkeleton = enabled;
    emit editSkeletonChanged();
}

bool EditorSession::isEditSkeleton() const
{
    return m_editSkeleton;
}

IEditableCourse * EditorSession::skeleton() const
{
    return m_skeleton;
}

void EditorSession::setSkeleton(IEditableCourse *skeleton)
{
    if (m_skeleton == skeleton) {
        return;
    }
    m_skeleton = skeleton;

    IEditableCourse *newCourse{ nullptr };
    if (m_skeleton && m_repository) {
        for (const auto &course : m_repository->editableCourses()) {
            if (course->foreignId() == m_skeleton->id()) {
                newCourse = course.get();
                break;
            }
        }
    }
    setCourse(newCourse);
    emit skeletonChanged();
}

ILanguage * EditorSession::language() const
{
    return m_language;
}

IEditableCourse * EditorSession::course() const
{
    return m_course;
}

void EditorSession::setCourse(IEditableCourse *course)
{
    if (m_course == course) {
        return;
    }
    m_course = course;

    if (m_course != nullptr) {
        // update skeleton
        IEditableCourse * newSkeleton{ nullptr };
        if (m_skeleton == nullptr || m_skeleton->id() != course->foreignId()) {
            for (const auto &skeleton : m_repository->skeletons()) {
                if (skeleton->id() == course->foreignId()) {
                    newSkeleton = skeleton.get();
                    break;
                }
            }
            m_skeleton = newSkeleton;
            emit skeletonChanged();
        }
        // update language
        m_language = m_course->language().get();
    } else {
        m_language = nullptr;
    }
    emit languageChanged();
    emit courseChanged();
}

void EditorSession::setCourseByLanguage(ILanguage *language)
{
    if (!skeletonMode()) {
        qDebug() << "Course selection by language is only available in skeleton mode";
        return;
    }
    if (language == nullptr || m_repository == nullptr) {
        return;
    }
    IEditableCourse *newCourse{ nullptr };
    QString languageId;
    if (language) {
        languageId = language->id();
    }
    for (auto course : m_repository->editableCourses()) {
        if (course->foreignId() == m_skeleton->id() && course->language()->id() == language->id()) {
            newCourse = course.get();
            break;
        }
    }
    setCourse(newCourse);
}

IEditableCourse * EditorSession::displayedCourse() const
{
    IEditableCourse * course{ nullptr };
    if (m_editSkeleton) {
        course = m_skeleton;
    } else {
        course = m_course;
    }
    return course;
}

void EditorSession::updateDisplayedUnit()
{
    auto course = displayedCourse();
    if (course != nullptr) {
        auto units = course->units();
        if (!units.isEmpty()) {
            setUnit(units.constFirst().get());
            return;
        }
    }
}

IUnit * EditorSession::unit() const
{
    return m_unit.get();
}

IUnit * EditorSession::activeUnit() const
{
    return m_unit.get();
}

void EditorSession::setUnit(IUnit *unit)
{
    if (unit != m_unit.get()) {
        if (!unit) {
            m_unit.reset();
        } else {
            m_unit = unit->self();
        }
        if (m_unit && !m_unit->phrases().isEmpty()) {
            setActivePhrase(m_unit->phrases().first().get());
        } else {
            setActivePhrase(nullptr);
        }
        emit unitChanged();
    }
}

void EditorSession::setActivePhrase(IPhrase * phrase)
{
    if (phrase && m_phrase == phrase->self()) {
        return;
    }
    if (phrase == nullptr) {
        m_phrase = nullptr;
    }
    else {
        setUnit(phrase->unit().get());
        m_phrase = phrase->self();
    }
    emit phraseChanged();
}

IPhrase * EditorSession::activePhrase() const
{
    return m_phrase.get();
}

std::shared_ptr<IPhrase> EditorSession::previousPhrase() const
{
    if (!m_phrase) {
        return nullptr;
    }
    const int index = m_phrase->unit()->phrases().indexOf(m_phrase);
    if (index > 0) {
        return m_phrase->unit()->phrases().at(index - 1);
    } else {
        auto unit = m_phrase->unit();
        int uIndex{ -1 };
        for (int i = 0; i < unit->course()->units().size(); ++i) {
            auto testUnit = unit->course()->units().at(i);
            if (testUnit.get() == unit.get()) {
                uIndex = i;
                break;
            }
        }
        if (uIndex > 0) {
            return unit->course()->units().at(uIndex - 1)->phrases().last();
        }
    }
    return m_phrase;
}

std::shared_ptr<IPhrase> EditorSession::nextPhrase() const
{
    if (!m_phrase) {
        return nullptr;
    }
    const int index = m_phrase->unit()->phrases().indexOf(m_phrase);
    if (index < m_phrase->unit()->phrases().length() - 1) {
        return m_phrase->unit()->phrases().at(index + 1);
    } else {
        auto unit = m_phrase->unit();
        int uIndex{ -1 };
        for (int i = 0; i < unit->course()->units().size(); ++i) {
            auto testUnit = unit->course()->units().at(i);
            if (testUnit.get() == unit.get()) {
                uIndex = i;
                break;
            }
        }
        if (uIndex < unit->course()->units().length() - 1) {
            auto nextUnit = unit->course()->units().at(uIndex + 1);
            if (nextUnit->phrases().isEmpty()) {
                return nullptr;
            }
            return nextUnit->phrases().constFirst();
        }
    }
    return m_phrase;
}

void EditorSession::switchToPreviousPhrase()
{
    if (hasPreviousPhrase()) {
        setActivePhrase(previousPhrase().get());
    }
}

void EditorSession::switchToNextPhrase()
{
    if (hasNextPhrase()) {
        setActivePhrase(nextPhrase().get());
    }
}

bool EditorSession::hasPreviousPhrase() const
{
    if (!m_unit || !m_phrase) {
        return false;
    }
    Q_ASSERT(m_unit->course() != nullptr);

    const int phraseIndex = m_phrase->unit()->phrases().indexOf(m_phrase);
    int unitIndex = -1;
    for (int i = 0; i < m_unit->course()->units().size(); ++i) {
        if (m_unit->course()->units().at(i) == m_unit) {
            unitIndex = i;
            break;
        }
    }
    if (unitIndex > 0 || phraseIndex > 0) {
        return true;
    }
    return false;
}

bool EditorSession::hasNextPhrase() const
{
    if (!m_unit || !m_phrase) {
        return false;
    }
    Q_ASSERT(m_unit->course() != nullptr);

    const int phraseIndex = m_phrase->unit()->phrases().indexOf(m_phrase);
    int unitIndex = -1;
    for (int i = 0; i < m_unit->course()->units().size(); ++i) {
        if (m_unit->course()->units().at(i) == m_unit) {
            unitIndex = i;
            break;
        }
    }
    if ((unitIndex >= 0 && unitIndex < m_course->units().size() - 1)
            || (phraseIndex >= 0 && phraseIndex < m_unit->phrases().size() - 1)) {
        return true;
    }
    return false;
}

void EditorSession::updateCourseFromSkeleton()
{
    if (!m_course) {
        qCritical() << "Not updating course from skeleton, no one set.";
        return;
    }
    m_repository->updateCourseFromSkeleton(m_course->self());
}
