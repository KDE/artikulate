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

#ifndef LEARNER_H
#define LEARNER_H

#include "liblearnerprofile_export.h"
#include <QObject>

class LearnerPrivate;

namespace LearnerProfile
{
class LearningGoal;

/**
 * \class Learner
 */
class LIBLEARNERPROFILE_EXPORT Learner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int id READ identifier WRITE setIdentifier NOTIFY identifierChanged)

public:
    explicit Learner(QObject *parent = 0);
    ~Learner();

    QString name() const;
    void setName(const QString &name);
    int identifier() const;
    void setIdentifier(int identifier);
    QList<LearningGoal *> goals() const;
    void addGoal(LearningGoal *goal);
    void removeGoal(LearningGoal *goal);

Q_SIGNALS:
    void nameChanged();
    void identifierChanged();
    void goalAdded(LearningGoal*,int);
    void goalAboutToBeRemoved(int);
    void goalRemoved();
    void goalCountChanged();

private:
    Q_DISABLE_COPY(Learner)
    const QScopedPointer<LearnerPrivate> d;
};
}

#endif // LEARNER_H
