/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "trainingsession.h"
#include "artikulate_debug.h"
#include "core/icourse.h"
#include "core/language.h"
#include "core/phrase.h"
#include "core/unit.h"
#include "learner.h"
#include "profilemanager.h"
#include "trainingaction.h"

TrainingSession::TrainingSession(LearnerProfile::ProfileManager *manager, QObject *parent)
    : ISessionActions(parent)
    , m_profileManager(manager)
    , m_course(nullptr)
{
    Q_ASSERT(m_profileManager != nullptr);
}

ICourse *TrainingSession::course() const
{
    return m_course;
}

void TrainingSession::setCourse(ICourse *course)
{
    if (!course) {
        updateTrainingActions();
        return;
    }
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
    for (auto unit : qAsConst(unitList)) {
        const auto phrases = unit->phrases();
        for (auto &phrase : phrases) {
            auto iter = data.find(phrase->id());
            if (iter != data.end()) {
                //                phrase->setProgress(iter.value()); //FIXME add a decorator?
            }
        }
    }
    updateTrainingActions();
    emit courseChanged();
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

TrainingAction *TrainingSession::activeAction() const
{
    if (m_indexUnit < 0 || m_indexPhrase < 0) {
        return nullptr;
    }
    return qobject_cast<TrainingAction *>(m_actions.at(m_indexUnit)->actions().at(m_indexPhrase));
}

IPhrase *TrainingSession::activePhrase() const
{
    if (const auto action = activeAction()) {
        return action->phrase();
    }
    return nullptr;
}

void TrainingSession::setActivePhrase(IPhrase *phrase)
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
    Q_ASSERT(m_indexUnit >= 0);
    Q_ASSERT(m_indexPhrase >= 0);
    if (m_indexUnit < 0 || m_indexPhrase < 0) {
        return;
    }

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
    if (auto action = activeAction()) {
        action->setChecked(false);
    }
    // try to find next phrase, otherwise return completed
    if (m_indexPhrase >= m_actions.at(m_indexUnit)->actions().count() - 1) {
        qDebug() << "switching to next unit";
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

bool TrainingSession::hasPrevious() const
{
    return m_indexUnit > 0 || m_indexPhrase > 0;
}

bool TrainingSession::hasNext() const
{
    if (m_indexUnit < m_actions.count() - 1) {
        return true;
    }
    if (!m_actions.isEmpty() && m_actions.constLast()) {
        if (m_indexPhrase < m_actions.constLast()->actions().count() - 1) {
            return true;
        }
    }
    return false;
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

QVector<TrainingAction *> TrainingSession::trainingActions() const
{
    return m_actions;
}

void TrainingSession::updateTrainingActions()
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
}
