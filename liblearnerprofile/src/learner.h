/*
 *  SPDX-FileCopyrightText: 2013-2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef LEARNER_H
#define LEARNER_H

#include "learninggoal.h"
#include "liblearnerprofile_export.h"
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
    Q_PROPERTY(QList<LearnerProfile::LearningGoal *> goals READ goals NOTIFY goalCountChanged)

public:
    // TODO workaround for QT-BUG-26415, fixed in Qt5.0
    // we must simulate the LearningGoal::Category enum in Learner to be able to allow its usage
    // as parameter for Q_INVOKABLE method
    // can be removed with Qt 5.0 migration
    Q_ENUMS(Category)
    enum Category { Unspecified = 0, Language = 1 };

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
    Q_INVOKABLE LearnerProfile::LearningGoal *activeGoal(LearnerProfile::Learner::Category category) const;

Q_SIGNALS:
    void nameChanged();
    void imageChanged();
    void identifierChanged();
    void goalAboutToBeAdded(LearningGoal *, int);
    void goalAdded();
    void goalAboutToBeRemoved(int);
    void goalRemoved(Learner *, LearningGoal *);
    void goalCountChanged();
    void activeGoalChanged();

private:
    Q_DISABLE_COPY(Learner)
    const QScopedPointer<LearnerPrivate> d;
};
}

#endif // LEARNER_H
