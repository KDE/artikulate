/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TRAININGSESSION_H
#define TRAININGSESSION_H

#include "isessionactions.h"
#include <QList>
#include <QQmlEngine>
#include <core/phrase.h>

class Language;
class ICourse;
class Unit;

namespace LearnerProfile
{
class ProfileManager;
}

/**
 * \class TrainingSession
 */
class TrainingSession : public ISessionActions
{
    Q_OBJECT
    Q_INTERFACES(ISessionActions)
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(IUnit *unit READ activeUnit WRITE setUnit NOTIFY phraseChanged)
    Q_PROPERTY(IPhrase *phrase READ activePhrase WRITE setActivePhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasNext READ hasNext NOTIFY phraseChanged)

    QML_ELEMENT
    QML_SINGLETON

public:
    explicit TrainingSession(QObject *parent = nullptr);
    explicit TrainingSession(LearnerProfile::ProfileManager *manager, QObject *parent = nullptr);

    ICourse *course() const;
    void setCourse(ICourse *course);
    IUnit *activeUnit() const;
    void setUnit(IUnit *unit);
    IPhrase *activePhrase() const override;
    void setActivePhrase(IPhrase *phrase) override;
    bool hasPrevious() const;
    bool hasNext() const;
    Q_INVOKABLE void accept();
    Q_INVOKABLE void skip();

Q_SIGNALS:
    /**
     * @brief Emitted when last phrase of session is skipped or marked as completed.
     */
    void completed();
    void closeUnit();

private:
    Q_DISABLE_COPY(TrainingSession)
    void selectNextPhrase();
    void updateGoal();
    LearnerProfile::ProfileManager *m_profileManager{nullptr};
    ICourse *m_course{nullptr};
    IPhrase *m_phrase{nullptr};
};

#endif
