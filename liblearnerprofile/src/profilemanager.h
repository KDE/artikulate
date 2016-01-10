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

#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include "liblearnerprofile_export.h"
#include "learninggoal.h"
#include <QObject>

namespace LearnerProfile
{
class ProfileManagerPrivate;
class Learner;
class LearningGoal;

/**
 * \class ProfileManager
 */
class LIBLEARNERPROFILE_EXPORT ProfileManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int profileCount READ profileCount NOTIFY profileCountChanged)
    Q_PROPERTY (LearnerProfile::Learner * activeProfile READ activeProfile WRITE setActiveProfile NOTIFY activeProfileChanged)

public:
    enum Progress {
        Skip = 0,
        Next = 1
    };

    explicit ProfileManager(QObject *parent = nullptr);
    virtual ~ProfileManager();

    QList< Learner* > profiles() const;
    int profileCount() const;
    Q_INVOKABLE LearnerProfile::Learner * addProfile(const QString &name);
    Q_INVOKABLE void removeProfile(LearnerProfile::Learner *learner);
    Q_INVOKABLE LearnerProfile::Learner * profile(int index);
    Q_INVOKABLE void openImageFileDialog();
    QList< LearningGoal* > goals() const;
    /**
     * Register learning goal if not registered yet. The registered goals will be stored at the
     * internal database.
     */
    void registerGoal(LearningGoal::Category category, const QString &identifier, const QString &name);
    LearningGoal * goal(LearningGoal::Category category, const QString &identifier) const;
    /**
     * updates current learning goal by activity, adds new learning goal if necessary,
     * stores log data for this activity
     */
    void recordProgress(Learner *learner, LearningGoal *goal,
                       const QString &container, const QString &item, int payload);
    /**
     * write all profiles to database
     */
    Q_INVOKABLE void sync();
    /**
     * write specified \p profile to database
     */
    Q_INVOKABLE void sync(LearnerProfile::Learner *learner);
    void setActiveProfile(LearnerProfile::Learner *learner);
    LearnerProfile::Learner * activeProfile() const;

Q_SIGNALS:
    void activeProfileChanged();
    void profileAdded(Learner*,int);
    void profileAboutToBeRemoved(int);
    void profileRemoved();
    void profileCountChanged();

private Q_SLOTS:
    void removeLearningGoal(Learner *learner, LearningGoal *goal);

private:
    Q_DISABLE_COPY(ProfileManager)
    const QScopedPointer<ProfileManagerPrivate> d;
};
}

#endif // PROFILEMANAGER_H
