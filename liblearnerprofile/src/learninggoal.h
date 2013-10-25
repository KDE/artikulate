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

public:
    Q_ENUMS(Type)
    enum PlaybackState {
        Unspecified = 0,
        Language = 1
    };

    explicit LearningGoal(QObject *parent = 0);
    ~LearningGoal();

    QString name() const;
    void setName(const QString &name);
    QString identifier() const;
    void setIdentifier(const QString &identifier);

Q_SIGNALS:
    void nameChanged();
    void identifierChanged();

private:
    Q_DISABLE_COPY(LearningGoal)
    const QScopedPointer<LearningGoalPrivate> d;
};
}

#endif // LEARINGGOAL_H
