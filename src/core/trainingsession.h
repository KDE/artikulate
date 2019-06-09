/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRAININGSESSION_H
#define TRAININGSESSION_H

#include "artikulatecore_export.h"
#include "phrase.h"
#include <QVector>

class Language;
class ICourse;
class Unit;
class TrainingAction;

namespace LearnerProfile {
    class ProfileManager;
}

/**
 * \class TrainingSession
 */
class ARTIKULATECORE_EXPORT TrainingSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(Unit *unit READ activeUnit WRITE setUnit NOTIFY phraseChanged)
    Q_PROPERTY(Phrase *phrase READ activePhrase WRITE setPhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasNext READ hasNext NOTIFY phraseChanged)

public:
    explicit TrainingSession(LearnerProfile::ProfileManager *manager, QObject *parent = nullptr);

    ICourse * course() const;
    void setCourse(ICourse *course);
    Unit * activeUnit() const;
    void setUnit(Unit *unit);
    TrainingAction * activeAction() const;
    Phrase * activePhrase() const;
    void setPhrase(Phrase *phrase);
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
    QVector<TrainingAction *> trainingActions() const;

Q_SIGNALS:
    void courseChanged();
    void phraseChanged();
    /**
     * @brief Emitted when last phrase of session is skipped or marked as completed.
     */
    void completed();

private:
    Q_DISABLE_COPY(TrainingSession)
    void updateTrainingActions();
    void selectNextPhrase();
    void updateGoal();
    LearnerProfile::ProfileManager *m_profileManager;
    ICourse *m_course;
    QVector<TrainingAction*> m_actions;

    int m_indexUnit{-1};
    int m_indexPhrase{-1};
};

#endif
