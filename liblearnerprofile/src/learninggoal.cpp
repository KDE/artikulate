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

#include "learninggoal_p.h"
#include "learninggoal.h"

using namespace LearnerProfile;

LearningGoal::LearningGoal(QObject *parent)
    : QObject(parent)
    , d(new LearningGoalPrivate)
{

}

QString LearningGoal::name() const
{
    return d->name;
}

LearningGoal::~LearningGoal()
{

}

void LearningGoal::setName(const QString &name)
{
    if (name == d->name) {
        return;
    }
    d->name = name;
    nameChanged();
}

QString LearningGoal::identifier() const
{
    return d->identifier;
}

void LearningGoal::setIdentifier(const QString &identifier)
{
    if (identifier == d->identifier) {
        return;
    }
    d->identifier = identifier;
    emit identifierChanged();
}
