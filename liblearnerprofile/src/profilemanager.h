/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include "learninggoal.h"
#include "liblearnerprofile_export.h"
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
    Q_PROPERTY(int profileCount READ profileCount NOTIFY profileCountChanged)
    Q_PROPERTY(LearnerProfile::Learner *activeProfile READ activeProfile WRITE setActiveProfile NOTIFY activeProfileChanged)

public:
    enum Progress { Skip = 0, Next = 1 };

    explicit ProfileManager(QObject *parent = nullptr);
    virtual ~ProfileManager();

    QList<Learner *> profiles() const;
    int profileCount() const;
    Q_INVOKABLE LearnerProfile::Learner *addProfile(const QString &name);
    Q_INVOKABLE void removeProfile(LearnerProfile::Learner *learner);
    Q_INVOKABLE LearnerProfile::Learner *profile(int index);
    Q_INVOKABLE void openImageFileDialog();
    QList<LearningGoal *> goals() const;
    /**
     * Register learning goal if not registered yet. The registered goals will be stored at the
     * internal database.
     */
    LearningGoal *registerGoal(LearningGoal::Category category, const QString &identifier, const QString &name);
    LearningGoal *goal(LearningGoal::Category category, const QString &identifier) const;
    /**
     * updates current learning goal by activity, adds new learning goal if necessary,
     * stores log data for this activity
     */
    void recordProgress(Learner *learner, LearningGoal *goal, const QString &container, const QString &item, int logPayload, int valuePayload);
    /**
     * \return progress value, or -1 if value is not available yet
     */
    QHash<QString, int> progressValues(Learner *learner, LearningGoal *goal, const QString &container) const;
    /**
     * write all profiles to database
     */
    Q_INVOKABLE void sync();
    /**
     * write specified \p profile to database
     */
    Q_INVOKABLE void sync(LearnerProfile::Learner *learner);
    void setActiveProfile(LearnerProfile::Learner *learner);
    LearnerProfile::Learner *activeProfile() const;

Q_SIGNALS:
    void activeProfileChanged();
    void profileAdded(Learner *, int);
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
