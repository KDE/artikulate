/*
 *  Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "learninggoal.h"
#include <QObject>

namespace LearnerProfile
{
class LearnerPrivate;
class LearningGoal;

/**
 * \class Learner
 */
class LIBLEARNERPROFILE_EXPORT Learner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int id READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(QString imageUrl READ imageUrl NOTIFY imageChanged)
    Q_PROPERTY(QList<LearnerProfile::LearningGoal*> goals READ goals NOTIFY goalCountChanged)

public:
    // TODO workaround for QT-BUG-26415, fixed in Qt5.0
    // we must simulate the LearningGoal::Category enum in Learner to be able to allow its usage
    // as parameter for Q_INVOKABLE method
    // can be removed with Qt 5.0 migration
    Q_ENUMS(Category)
    enum Category {
        Unspecified = 0,
        Language = 1
    };

    explicit Learner(QObject *parent = nullptr);
    ~Learner();

    QString name() const;
    void setName(const QString &name);
    /**
     * \return URL to image
     * \note since it is a local file the path begins with "file://"
     */
    QString imageUrl() const;
    Q_INVOKABLE void clearImage();
    void importImage(const QString &path);
    int identifier() const;
    void setIdentifier(int identifier);
    /**
     * \return list of all learning goals of learner
     */
    QList<LearningGoal *> goals() const;
    Q_INVOKABLE void addGoal(LearnerProfile::LearningGoal *goal);
    Q_INVOKABLE void removeGoal(LearnerProfile::LearningGoal *goal);
    Q_INVOKABLE bool hasGoal(LearnerProfile::LearningGoal *goal) const;
    void setActiveGoal(LearnerProfile::LearningGoal *goal);
    Q_INVOKABLE void setActiveGoal(LearnerProfile::Learner::Category category, const QString &identifier);
    Q_INVOKABLE LearnerProfile::LearningGoal * activeGoal(LearnerProfile::Learner::Category category) const;

Q_SIGNALS:
    void nameChanged();
    void imageChanged();
    void identifierChanged();
    void goalAboutToBeAdded(LearningGoal*,int);
    void goalAdded();
    void goalAboutToBeRemoved(int);
    void goalRemoved(Learner*, LearningGoal*);
    void goalCountChanged();
    void activeGoalChanged();

private:
    Q_DISABLE_COPY(Learner)
    const QScopedPointer<LearnerPrivate> d;
};
}

#endif // LEARNER_H
