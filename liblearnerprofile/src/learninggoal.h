/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef LEARNINGGOAL_H
#define LEARNINGGOAL_H

#include "liblearnerprofile_export.h"
#include <QObject>

class LearningGoalPrivate;

namespace LearnerProfile
{
/**
 * \class LearningGoal
 */
class LIBLEARNERPROFILE_EXPORT LearningGoal : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString id READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(Category category READ category)

public:
    Q_ENUMS(Category)
    enum Category { Unspecified = 0, Language = 1 };

    explicit LearningGoal(QObject *parent = nullptr);
    explicit LearningGoal(LearningGoal::Category category, const QString &identifier, QObject *parent = nullptr);
    ~LearningGoal();

    QString name() const;
    void setName(const QString &name);
    QString identifier() const;
    void setIdentifier(const QString &identifier);
    LearningGoal::Category category() const;

Q_SIGNALS:
    void nameChanged();
    void identifierChanged();

private:
    Q_DISABLE_COPY(LearningGoal)
    const QScopedPointer<LearningGoalPrivate> d;
};
}

#endif // LEARNINGGOAL_H
