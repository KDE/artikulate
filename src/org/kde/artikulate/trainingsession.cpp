/*
    SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "trainingsession.h"
#include "artikulate_debug.h"
#include "learner.h"
#include "profilemanager.h"
#include <core/icourse.h>
#include <core/language.h>
#include <core/phrase.h>
#include <core/unit.h>

TrainingSession::TrainingSession(QObject *parent)
    : ISessionActions(parent)
    , m_profileManager(new LearnerProfile::ProfileManager(this))
{
    Q_ASSERT(m_profileManager != nullptr);
}

TrainingSession::TrainingSession(LearnerProfile::ProfileManager *manager, QObject *parent)
    : ISessionActions(parent)
    , m_profileManager(manager)
{
    Q_ASSERT(m_profileManager != nullptr);
}

ICourse *TrainingSession::course() const
{
    return m_course;
}

void TrainingSession::setCourse(ICourse *course)
{
    if (m_course == course) {
        return;
    }
    m_course = course;
    if (m_course && m_course->units().count() > 0) {
        setUnit(m_course->units().constFirst().get());
    }

    // lazy loading of training data
    LearnerProfile::LearningGoal *goal = m_profileManager->goal(LearnerProfile::LearningGoal::Language, m_course->id());
    if (!goal) {
        goal = m_profileManager->registerGoal(LearnerProfile::LearningGoal::Language, course->language()->id(), course->language()->i18nTitle());
    }
    auto data = m_profileManager->progressValues(m_profileManager->activeProfile(), goal, m_course->id());
    const auto unitList = m_course->units();
    for (const auto &unit : unitList) {
        const auto phrases = unit->phrases();
        for (auto &phrase : phrases) {
            auto iter = data.find(phrase->id());
            if (iter != data.end()) {
                //                phrase->setProgress(iter.value()); //FIXME add a decorator?
            }
        }
    }
    Q_EMIT courseChanged();
}

IUnit *TrainingSession::activeUnit() const
{
    if (auto phrase = activePhrase()) {
        return phrase->unit().get();
    }
    return nullptr;
}

void TrainingSession::setUnit(IUnit *unit)
{
    if (!unit) {
        return;
    }
    for (const auto &phrase : unit->phrases()) {
        if (!phrase->sound().isEmpty()) {
            setActivePhrase(phrase.get());
            return;
        }
    }
}

IPhrase *TrainingSession::activePhrase() const
{
    return m_phrase;
}

void TrainingSession::setActivePhrase(IPhrase *phrase)
{
    if (phrase == m_phrase) {
        return;
    }
    m_phrase = phrase;
    Q_EMIT phraseChanged();
}

void TrainingSession::accept()
{
    auto phrase = activePhrase();

    // possibly update goals of learner
    updateGoal();
    //    phrase->updateProgress(Phrase::Progress::Done); //FIXME

    // store training activity
    LearnerProfile::LearningGoal *goal = m_profileManager->goal(LearnerProfile::LearningGoal::Language, m_course->language()->id());
    //    m_profileManager->recordProgress(m_profileManager->activeProfile(), //FIXME
    //        goal,
    //        m_course->id(),
    //        phrase->id(),
    //        static_cast<int>(LearnerProfile::ProfileManager::Skip),
    //        phrase->progress()
    //    );

    selectNextPhrase();
}

void TrainingSession::skip()
{
    // possibly update goals of learner
    updateGoal();
    auto phrase = activePhrase();
    //    phrase->updateProgress(Phrase::Progress::Skip); //FIXME

    // store training activity
    LearnerProfile::LearningGoal *goal = m_profileManager->goal(LearnerProfile::LearningGoal::Language, m_course->language()->id());
    //    m_profileManager->recordProgress(m_profileManager->activeProfile(),
    //        goal,
    //        m_course->id(),
    //        phrase->id(),
    //        static_cast<int>(LearnerProfile::ProfileManager::Skip),
    //        phrase->progress()
    //    ); // FIXME

    selectNextPhrase();
}

void TrainingSession::selectNextPhrase()
{
    if (!m_phrase) {
        qWarning() << "Cannot select next phrase, no current phrase set";
        return;
    }
    m_phrase = m_phrase->next().get();
    Q_EMIT phraseChanged();
}

bool TrainingSession::hasPrevious() const
{
    return m_phrase != nullptr && m_phrase->previous() != nullptr;
}

bool TrainingSession::hasNext() const
{
    return m_phrase != nullptr && m_phrase->next() != nullptr;
}

void TrainingSession::updateGoal()
{
    if (!m_profileManager) {
        qCWarning(ARTIKULATE_LOG()) << "No ProfileManager registered, aborting operation";
        return;
    }
    LearnerProfile::Learner *learner = m_profileManager->activeProfile();
    if (!learner) {
        qCWarning(ARTIKULATE_LOG()) << "No active Learner registered, aborting operation";
        return;
    }
    LearnerProfile::LearningGoal *goal = m_profileManager->goal(LearnerProfile::LearningGoal::Language, m_course->language()->id());
    learner->addGoal(goal);
    learner->setActiveGoal(goal);
}
