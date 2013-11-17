/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "learner_p.h"
#include "learner.h"
#include "learninggoal.h"
#include <KDebug>

using namespace LearnerProfile;

Learner::Learner(QObject *parent)
    : QObject(parent)
    , d(new LearnerPrivate)
{
    connect(this, SIGNAL(goalAdded(LearningGoal*,int)), this, SIGNAL(goalCountChanged()));
    connect(this, SIGNAL(goalRemoved()), this, SIGNAL(goalCountChanged()));
}

Learner::~Learner()
{

}

QString Learner::name() const
{
    return d->m_name;
}

void Learner::setName(const QString &name)
{
    if (name == d->m_name) {
        return;
    }
    d->m_name = name;
    emit nameChanged();
}

int Learner::identifier() const
{
    return d->m_identifier;
}

void Learner::setIdentifier(int identifier)
{
    if (identifier == d->m_identifier) {
        return;
    }
    d->m_identifier = identifier;
    emit identifierChanged();
}

QList< LearningGoal* > Learner::goals() const
{
    return d->m_goals;
}

void Learner::addGoal(LearnerProfile::LearningGoal* goal)
{
    d->m_goals.append(goal);
    emit goalAdded(goal, d->m_goals.count() - 1);
}

void Learner::removeGoal(LearnerProfile::LearningGoal* goal)
{
    int index = d->m_goals.indexOf(goal);
    if (index < 0) {
        kError() << "Cannot remove goal, not found: aborting";
        return;
    }
    emit goalAboutToBeRemoved(index);
    d->m_goals.removeAt(index);
    emit goalRemoved();
}

void Learner::setActiveGoal(LearningGoal *goal)
{
    d->m_activeGoal.insert(goal->category(), goal);
    emit activeGoalChanged();
}

void Learner::setActiveGoal(Learner::Category categoryLearner, const QString &identifier)
{
    // TODO:Qt5 change method parameter to LearningGoal::Category
    // workaround for Q_INVOKABLE access of enum
    LearningGoal::Category category = static_cast<LearningGoal::Category>(categoryLearner);
    foreach (LearningGoal *goal, d->m_goals) {
        if (goal->category() == category && goal->identifier() == identifier) {
            d->m_activeGoal[category] = goal;
            return;
        }
    }
    kError() << "Could not select learning goal with ID " << identifier
        << ": not registered for this learner";
}

LearningGoal * Learner::activeGoal(Learner::Category categoryLearner) const
{
    // TODO:Qt5 change method parameter to LearningGoal::Category
    // workaround for Q_INVOKABLE access of enum
    LearningGoal::Category category = static_cast<LearningGoal::Category>(categoryLearner);
    if (d->m_activeGoal.contains(category)) {
        kWarning() << "No current learning goal set for this category: fall back to first in list";
        foreach (LearningGoal *goal, d->m_goals) {
            if (goal->category() == category) {
                return goal;
            }
        }
        kWarning() << "No learning goals of catagory " << category << " registered";
        return 0;
    }

    return d->m_activeGoal[category];
}
