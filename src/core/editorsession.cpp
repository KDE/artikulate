/*
 *  SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "editorsession.h"
#include "artikulate_debug.h"
#include "core/contributorrepository.h"
#include "core/iunit.h"
#include "core/language.h"
#include "core/phrase.h"
#include "core/resources/editablecourseresource.h"
#include "core/resources/skeletonresource.h"
#include "core/trainingaction.h"
#include "core/unit.h"

EditorSession::EditorSession(QObject *parent)
    : ISessionActions(parent)
{
    connect(this, &EditorSession::courseChanged, this, &EditorSession::skeletonModeChanged);
}

void EditorSession::setRepository(IEditableRepository *repository)
{
    m_repository = repository;
}

bool EditorSession::skeletonMode() const
{
    for (const auto &skeleton : m_repository->skeletons()) {
        if (skeleton->id() == m_course->id()) {
            return true;
        }
    }
    return false;
}

ILanguage *EditorSession::language() const
{
    if (m_course && m_course->language()) {
        return m_course->language().get();
    }
    return nullptr;
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

    connect(course, &IEditableCourse::unitChanged, this, [=](std::shared_ptr<IEditableUnit> unit) {
        this->updateActions(unit);
        emit actionsChanged(); // TODO much too global effect
    });
    updateTrainingActions();
    if (m_course && m_course->units().count() > 0) {
        setActiveUnit(m_course->units().first().get());
    }
    emit languageChanged();
    emit courseChanged();
}

IUnit *EditorSession::activeUnit() const
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
            const auto testPhrase = qobject_cast<TrainingAction *>(m_actions.at(i)->actions().at(j))->phrase();
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

void EditorSession::setActivePhrase(IPhrase *phrase)
{
    for (int i = 0; i < m_actions.count(); ++i) {
        for (int j = 0; j < m_actions.at(i)->actions().count(); ++j) {
            const auto testPhrase = qobject_cast<TrainingAction *>(m_actions.at(i)->actions().at(j))->phrase();
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

IPhrase *EditorSession::activePhrase() const
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

TrainingAction *EditorSession::activeAction() const
{
    if (m_indexUnit < 0 || m_indexPhrase < 0) {
        return nullptr;
    }
    return qobject_cast<TrainingAction *>(m_actions.at(m_indexUnit)->actions().at(m_indexPhrase));
}

void EditorSession::updateTrainingActions()
{
    for (const auto &action : qAsConst(m_actions)) {
        action->clearActions();
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
            action->appendAction(new TrainingAction(phrase, this, unit.get()));
        }
        if (action->actions().count() > 0) {
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
    emit actionsChanged();
}

void EditorSession::updateActions(std::shared_ptr<IEditableUnit> changedUnit)
{
    int unitIndex = -1;
    for (int i = 0; i < m_course->units().size(); ++i) {
        if (changedUnit == m_course->units().at(i)) {
            unitIndex = i;
            break;
        }
    }
    Q_ASSERT(unitIndex >= 0);
    auto unitAction = m_actions.at(unitIndex);

    // TODO this is a heavy operation if only one phrase was changed
    unitAction->clearActions();
    for (int i = 0; i < changedUnit->phrases().size(); ++i) {
        unitAction->appendAction(new TrainingAction(changedUnit->phrases().at(i), this, changedUnit.get()));
    }

    emit unitAction->actionsChanged();
}

QVector<TrainingAction *> EditorSession::trainingActions() const
{
    return m_actions;
}
