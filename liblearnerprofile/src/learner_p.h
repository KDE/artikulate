/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef LEARNER_P_H
#define LEARNER_P_H

#include <QString>
#include <QList>
#include <QHash>
#include <QStandardPaths>
#include "learninggoal.h"
#include <QDebug>

namespace LearnerProfile
{
class LearningGoal;

class LearnerPrivate {

public:
    LearnerPrivate()
        : m_name(QString())
        , m_identifier(-1)
    {
    }
    ~LearnerPrivate() {}

    QString imagePath() const
    {
        const QString name = QString("learner%1.png").arg(m_identifier);
        return imageDirectory() + name;
    }
    QString imageDirectory() const
    {
        return QStandardPaths::writableLocation(QStandardPaths::DataLocation)
            + QLatin1Char('/')
            + QString("images")
            + QLatin1Char('/');
    }

    QString m_name;
    int m_identifier;
    QList<LearningGoal*> m_goals;
    QHash<LearningGoal::Category, LearningGoal*> m_activeGoal;
};
}

#endif // LEARNER_P_H
