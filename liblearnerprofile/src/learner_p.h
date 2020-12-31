/*
    SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef LEARNER_P_H
#define LEARNER_P_H

#include "learninggoal.h"
#include <QDebug>
#include <QHash>
#include <QList>
#include <QStandardPaths>
#include <QString>

namespace LearnerProfile
{
class LearningGoal;

class LearnerPrivate
{
public:
    LearnerPrivate()
        : m_name(QString())
        , m_identifier(-1)
    {
    }
    ~LearnerPrivate()
    {
    }

    QString imagePath() const
    {
        const QString name = QStringLiteral("learner%1.png").arg(m_identifier);
        return imageDirectory() + name;
    }
    QString imageDirectory() const
    {
        return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') + QStringLiteral("images") + QLatin1Char('/');
    }

    QString m_name;
    int m_identifier;
    QList<LearningGoal *> m_goals;
    QHash<LearningGoal::Category, LearningGoal *> m_activeGoal;
};
}

#endif // LEARNER_P_H
