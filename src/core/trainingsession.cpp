/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "core/icourse.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "profilemanager.h"
#include "learner.h"
#include "trainingaction.h"
#include "artikulate_debug.h"

TrainingSession::TrainingSession(QObject *parent)
    : QObject(parent)
    , m_profileManager(nullptr)
    , m_course(nullptr)
    , m_unit(nullptr)
{

}

void TrainingSession::setProfileManager(LearnerProfile::ProfileManager *manager)
{
    if (m_profileManager == manager) {
        return;
    }
    m_profileManager = manager;
}

ICourse * TrainingSession::course() const
{
    return m_course;
}

void TrainingSession::setCourse(ICourse *course)
{
    if (!course) {
        return;
    }
    if (m_course == course) {
        return;
    }
    m_course = course;
    if (m_course && m_course->unitList().count() > 0) {
        setUnit(m_course->unitList().constFirst());
    }

    // lazy loading of training data
    LearnerProfile::LearningGoal * goal = m_profileManager->goal(
        LearnerProfile::LearningGoal::Language, m_course->id());
    if (!goal) {
        goal = m_profileManager->registerGoal(
            LearnerProfile::LearningGoal::Language,
            course->language()->id(),
            course->language()->i18nTitle()
        );
    }
    auto data = m_profileManager->progressValues(m_profileManager->activeProfile(),
        goal,
        m_course->id()
    );
    Q_FOREACH(Unit *unit, m_course->unitList()) {
        Q_FOREACH(Phrase *phrase, unit->phraseList()) {
            auto iter = data.find(phrase->id());
            if (iter != data.end()) {
                phrase->setProgress(iter.value());
            }
        }
    }

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
    if (m_unit && m_unit->phraseList().count() > 0) {
        setPhrase(m_unit->phraseList().constFirst());
    }
    emit unitChanged();
}

TrainingAction * TrainingSession::activeAction() const
{
    if (m_indexUnit < 0 || m_indexPhrase < 0) {
        return nullptr;
    }
    return qobject_cast<TrainingAction*>(m_actions.at(m_indexUnit)->actions().at(m_indexPhrase));
}

Phrase * TrainingSession::activePhrase() const
{
    if (const auto action = activeAction()) {
        return action->phrase();
    }
    return nullptr;
}

void TrainingSession::setPhrase(Phrase *phrase)
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

void TrainingSession::accept()
{
    Q_ASSERT(m_indexUnit >= 0);
    Q_ASSERT(m_indexPhrase >= 0);
    if (m_indexUnit < 0 || m_indexPhrase < 0) {
        return;
    }
    auto phrase = activePhrase();

    // possibly update goals of learner
    updateGoal();
    phrase->updateProgress(Phrase::Progress::Done);

    // store training activity
    LearnerProfile::LearningGoal * goal = m_profileManager->goal(
        LearnerProfile::LearningGoal::Language, m_course->language()->id());
    m_profileManager->recordProgress(m_profileManager->activeProfile(),
        goal,
        m_course->id(),
        phrase->id(),
        static_cast<int>(LearnerProfile::ProfileManager::Skip),
        phrase->progress()
    );

    selectNextPhrase();
}

void TrainingSession::skip()
{
    Q_ASSERT(m_indexUnit >= 0);
    Q_ASSERT(m_indexPhrase >= 0);
    if (m_indexUnit < 0 || m_indexPhrase < 0) {
        return;
    }

    // possibly update goals of learner
    updateGoal();
    auto phrase = activePhrase();
    phrase->updateProgress(Phrase::Progress::Skip);

    // store training activity
    LearnerProfile::LearningGoal * goal = m_profileManager->goal(
        LearnerProfile::LearningGoal::Language, m_course->language()->id());
    m_profileManager->recordProgress(m_profileManager->activeProfile(),
        goal,
        m_course->id(),
        phrase->id(),
        static_cast<int>(LearnerProfile::ProfileManager::Skip),
        phrase->progress()
    );

    selectNextPhrase();
}

void TrainingSession::selectNextPhrase()
{
    if (auto action = activeAction()) {
        action->setChecked(false);
    }
    // try to find next phrase, otherwise return completed
    if (m_indexPhrase >= m_actions.at(m_indexUnit)->actions().count() - 1) {
        if (m_indexUnit >= m_actions.count() - 1) {
            emit completed();
        } else {
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
}

bool TrainingSession::hasNext() const
{
    return m_indexUnit < m_actions.count() - 1 || m_indexPhrase < m_actions.last()->actions().count() - 1;
}

void TrainingSession::updateGoal()
{
    if (!m_profileManager) {
        qCWarning(ARTIKULATE_LOG) << "No ProfileManager registered, aborting operation";
        return;
    }
    LearnerProfile::Learner *learner = m_profileManager->activeProfile();
    if (!learner) {
        qCWarning(ARTIKULATE_LOG) << "No active Learner registered, aborting operation";
        return;
    }
    LearnerProfile::LearningGoal * goal = m_profileManager->goal(
        LearnerProfile::LearningGoal::Language, m_course->language()->id());
    learner->addGoal(goal);
    learner->setActiveGoal(goal);
}

QVector<TrainingAction *> TrainingSession::trainingActions()
{
    // cleanup
    for (const auto &action : qAsConst(m_actions)) {
        action->deleteLater();
    }
    m_actions.clear();

    if (!m_course) {
        return QVector<TrainingAction *>();
    }

    const auto unitList = m_course->unitList();
    for (const auto &unit : qAsConst(unitList)) {
        auto action = new TrainingAction(unit->title());
        const auto phraseList = unit->phraseList();
        for (const auto &phrase : qAsConst(phraseList)) {
            if (phrase->sound().isEmpty()) {
                continue;
            }
            action->appendChild(new TrainingAction(phrase, this, unit));
        }
        if (action->hasChildren()) {
            m_actions.append(action);
        } else {
            action->deleteLater();
        }
    }
    return m_actions;
}
