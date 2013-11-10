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
    return d->name;
}

void Learner::setName(const QString &name)
{
    if (name == d->name) {
        return;
    }
    d->name = name;
    emit nameChanged();
}

int Learner::identifier() const
{
    return d->identifier;
}

void Learner::setIdentifier(int identifier)
{
    if (identifier == d->identifier) {
        return;
    }
    d->identifier = identifier;
    emit identifierChanged();
}

QList< LearningGoal* > Learner::goals() const
{
    return d->goals;
}

void Learner::addGoal(LearnerProfile::LearningGoal* goal)
{
    d->goals.append(goal);
    emit goalAdded(goal, d->goals.count() - 1);
}

void Learner::removeGoal(LearnerProfile::LearningGoal* goal)
{
    int index = d->goals.indexOf(goal);
    if (index < 0) {
        kError() << "Cannot remove goal, not found: aborting";
        return;
    }
    emit goalAboutToBeRemoved(index);
    d->goals.removeAt(index);
    emit goalRemoved();
}
