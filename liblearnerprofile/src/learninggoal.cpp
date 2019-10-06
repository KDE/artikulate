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

#include "learninggoal.h"

using namespace LearnerProfile;

// private class LearningGoalPrivate
class LearningGoalPrivate
{
public:
    LearningGoalPrivate()
        : m_name(QString())
        , m_identifier(QString())
        , m_category(LearningGoal::Unspecified)
    {
    }
    ~LearningGoalPrivate()
    {
    }

    QString m_name;
    QString m_identifier;
    LearningGoal::Category m_category;
};

// methods of LearningGoal
LearningGoal::LearningGoal(QObject *parent)
    : QObject(parent)
    , d(new LearningGoalPrivate)
{
    // create goal of unspecified category
}

LearningGoal::LearningGoal(LearningGoal::Category category, const QString &identifier, QObject *parent)
    : QObject(parent)
    , d(new LearningGoalPrivate)
{
    d->m_category = category;
    d->m_identifier = identifier;
}

QString LearningGoal::name() const
{
    return d->m_name;
}

LearningGoal::~LearningGoal()
{
}

void LearningGoal::setName(const QString &name)
{
    if (name == d->m_name) {
        return;
    }
    d->m_name = name;
    emit nameChanged();
}

QString LearningGoal::identifier() const
{
    return d->m_identifier;
}

void LearningGoal::setIdentifier(const QString &identifier)
{
    if (identifier == d->m_identifier) {
        return;
    }
    d->m_identifier = identifier;
    emit identifierChanged();
}

LearningGoal::Category LearningGoal::category() const
{
    return d->m_category;
}
