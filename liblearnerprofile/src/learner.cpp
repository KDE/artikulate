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

using namespace LearnerProfile;

Learner::Learner(QObject *parent)
    : QObject(parent)
    , d(new LearnerPrivate)
{

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

QString Learner::identifier() const
{
    return d->identifier;
}

void Learner::setIdentifier(const QString& identifier)
{
    if (identifier == d->identifier) {
        return;
    }
    d->identifier = identifier;
    emit identifierChanged();
}
