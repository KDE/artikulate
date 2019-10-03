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
#include "core/trainingaction.h"
#include "core/contributorrepository.h"
#include "artikulate_debug.h"

EditorSession::EditorSession(QObject *parent)
    : ISessionActions(parent)
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

IEditableCourse *EditorSession::course() const
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
    updateTrainingActions();
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
            setActiveUnit(units.constFirst().get());
            return;
        }
    }
}

IUnit * EditorSession::activeUnit() const
{
    if (auto phrase = activePhrase()) {
        return phrase->unit().get();
    }
    return nullptr;
}

void EditorSession::setActiveUnit(IUnit *unit)
{
    // checking phrases in increasing order ensures that always the first phrase is selected
    for (int i = 0; i < m_actions.count(); ++i) {
        for (int j = 0; j < m_actions.at(i)->actions().count(); ++j) {
            const auto testPhrase = qobject_cast<TrainingAction*>(m_actions.at(i)->actions().at(j))->phrase();
            if (unit == testPhrase->unit().get()) {
                if (auto action = activeAction()) {
                    action->setChecked(false);
                }
                m_indexUnit = i;
                m_indexPhrase = j;
                if (auto action = activeAction()) {
                    action->setChecked(true);
                }
                emit phraseChanged();
                return;
            }
        }
    }
}

void EditorSession::setActivePhrase(IPhrase * phrase)
{
    for (int i = 0; i < m_actions.count(); ++i) {
        for (int j = 0; j < m_actions.at(i)->actions().count(); ++j) {
            const auto testPhrase = qobject_cast<TrainingAction*>(m_actions.at(i)->actions().at(j))->phrase();
            if (phrase == testPhrase) {
                if (auto action = activeAction()) {
                    action->setChecked(false);
                }
                m_indexUnit = i;
                m_indexPhrase = j;
                if (auto action = activeAction()) {
                    action->setChecked(true);
                }
                emit phraseChanged();
                return;
            }
        }
    }
}

IPhrase * EditorSession::activePhrase() const
{
    if (const auto action = activeAction()) {
        return action->phrase();
    }
    return nullptr;
}

void EditorSession::switchToPreviousPhrase()
{
    if (hasPreviousPhrase()) {
        if (m_indexPhrase == 0) {
            qCDebug(ARTIKULATE_CORE()) << "switching to previous unit";
            if (m_indexUnit > 0) {
                --m_indexUnit;
                m_indexPhrase = m_actions.at(m_indexUnit)->actions().count() - 1;
            }
        } else {
            --m_indexPhrase;
        }
        if (auto action = activeAction()) {
            action->setChecked(true);
        }
        emit phraseChanged();
    } else {
      qCWarning(ARTIKULATE_CORE()) << "The is no previous phrase, aborting";
    }
}

void EditorSession::switchToNextPhrase()
{
    if (hasNextPhrase()) {
        if (m_indexPhrase >= m_actions.at(m_indexUnit)->actions().count() - 1) {
            qCDebug(ARTIKULATE_CORE()) << "switching to next unit";
            if (m_indexUnit < m_actions.count() - 1) {
                ++m_indexUnit;
                m_indexPhrase = 0;
            }
        } else {
            ++m_indexPhrase;
        }
        if (auto action = activeAction()) {
            action->setChecked(true);
        }
        emit phraseChanged();
    } else {
      qCWarning(ARTIKULATE_CORE()) << "The is no next phrase, aborting";
    }
}

bool EditorSession::hasPreviousPhrase() const
{
    return m_indexUnit > 0 || m_indexPhrase > 0;
}

bool EditorSession::hasNextPhrase() const
{
    if (m_indexUnit < m_actions.count() - 1) {
        return true;
    }
    if (m_actions.constLast()) {
        if (m_indexPhrase < m_actions.constLast()->actions().count() - 1) {
            return true;
        }
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

TrainingAction * EditorSession::activeAction() const
{
    if (m_indexUnit < 0 || m_indexPhrase < 0) {
        return nullptr;
    }
    return qobject_cast<TrainingAction*>(m_actions.at(m_indexUnit)->actions().at(m_indexPhrase));
}

void EditorSession::updateTrainingActions()
{
    for (const auto &action : qAsConst(m_actions)) {
        action->deleteLater();
    }
    m_actions.clear();

    if (!m_course) {
        m_indexUnit = -1;
        m_indexPhrase = -1;
        return;
    }

    const auto unitList = m_course->units();
    for (const auto &unit : qAsConst(unitList)) {
        auto action = new TrainingAction(unit->title(), this);
        const auto phraseList = unit->phrases();
        for (const auto &phrase : qAsConst(phraseList)) {
            if (phrase->sound().isEmpty()) {
                continue;
            }
            action->appendChild(new TrainingAction(phrase, this, unit.get()));
        }
        if (action->hasChildren()) {
            m_actions.append(action);
        } else {
            action->deleteLater();
        }
    }

    // update indices
    m_indexUnit = -1;
    m_indexPhrase = -1;
    if (m_course->units().count() > 0) {
        m_indexUnit = 0;
        if (m_course->units().constFirst()->phrases().count() > 0) {
            m_indexPhrase = 0;
        }
    }
}

QVector<TrainingAction *> EditorSession::trainingActions() const
{
    return m_actions;
}
