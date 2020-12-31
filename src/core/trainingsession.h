/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TRAININGSESSION_H
#define TRAININGSESSION_H

#include "artikulatecore_export.h"
#include "isessionactions.h"
#include "phrase.h"
#include <QVector>

class Language;
class ICourse;
class Unit;
class TrainingAction;

namespace LearnerProfile
{
class ProfileManager;
}

/**
 * \class TrainingSession
 */
class ARTIKULATECORE_EXPORT TrainingSession : public ISessionActions
{
    Q_OBJECT
    Q_INTERFACES(ISessionActions)
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(IUnit *unit READ activeUnit WRITE setUnit NOTIFY phraseChanged)
    Q_PROPERTY(IPhrase *phrase READ activePhrase WRITE setActivePhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasNext READ hasNext NOTIFY phraseChanged)

public:
    explicit TrainingSession(LearnerProfile::ProfileManager *manager, QObject *parent = nullptr);

    ICourse *course() const;
    void setCourse(ICourse *course);
    IUnit *activeUnit() const;
    void setUnit(IUnit *unit);
    TrainingAction *activeAction() const override;
    IPhrase *activePhrase() const;
    void setActivePhrase(IPhrase *phrase) override;
    bool hasPrevious() const;
    bool hasNext() const;
    Q_INVOKABLE void accept();
    Q_INVOKABLE void skip();
    /**
     * @brief Return tree of training actions
     *
     * The return actions form a 2-level hierarchy:
     * - the first level are all units
     * - the unit actions may contain sub-actions, which are the phrases
     *
     * @note phrases without sound file paths are skipped when generating actions
     */
    QVector<TrainingAction *> trainingActions() const override;

Q_SIGNALS:
    /**
     * @brief Emitted when last phrase of session is skipped or marked as completed.
     */
    void completed();
    void closeUnit();

private:
    Q_DISABLE_COPY(TrainingSession)
    void updateTrainingActions();
    void selectNextPhrase();
    void updateGoal();
    LearnerProfile::ProfileManager *m_profileManager;
    ICourse *m_course;
    QVector<TrainingAction *> m_actions;

    int m_indexUnit {-1};
    int m_indexPhrase {-1};
};

#endif
