/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
