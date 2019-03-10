/*
 *  Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "profilemanager.h"
#include "learner.h"
#include "artikulate_debug.h"

TrainingSession::TrainingSession(QObject *parent)
    : QObject(parent)
    , m_profileManager(nullptr)
    , m_course(nullptr)
    , m_unit(nullptr)
    , m_phrase(nullptr)
{

}

void TrainingSession::setProfileManager(LearnerProfile::ProfileManager *manager)
{
    if (m_profileManager == manager) {
        return;
    }
    m_profileManager = manager;
}

Course * TrainingSession::course() const
{
    return m_course;
}

void TrainingSession::setCourse(Course *course)
{
    if (!course) {
        return;
    }
    if (m_course == course) {
        return;
    }
    m_course = course;
    if (m_course && m_course->unitList().count() > 0) {
        setUnit(m_course->unitList().first());
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
        setPhrase(m_unit->phraseList().first());
    }
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

Phrase * TrainingSession::nextPhrase() const
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

void TrainingSession::showNextPhrase()
{
    // possibly update goals of learner
    updateGoal();
    m_phrase->updateProgress(Phrase::Progress::Done);

    // store training activity
    LearnerProfile::LearningGoal * goal = m_profileManager->goal(
        LearnerProfile::LearningGoal::Language, m_course->language()->id());
    m_profileManager->recordProgress(m_profileManager->activeProfile(),
        goal,
        m_course->id(),
        m_phrase->id(),
        static_cast<int>(LearnerProfile::ProfileManager::Skip),
        m_phrase->progress()
    );

    setPhrase(nextPhrase());
}

void TrainingSession::skipPhrase()
{
    // possibly update goals of learner
    updateGoal();
    m_phrase->updateProgress(Phrase::Progress::Skip);

    // store training activity
    LearnerProfile::LearningGoal * goal = m_profileManager->goal(
        LearnerProfile::LearningGoal::Language, m_course->language()->id());
    m_profileManager->recordProgress(m_profileManager->activeProfile(),
        goal,
        m_course->id(),
        m_phrase->id(),
        static_cast<int>(LearnerProfile::ProfileManager::Skip),
        m_phrase->progress()
    );

    setPhrase(nextPhrase());
}

bool TrainingSession::hasNextPhrase() const
{
    return nextPhrase() != nullptr;
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
