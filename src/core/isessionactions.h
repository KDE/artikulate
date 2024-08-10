/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef ISESSIONACTIONS_H
#define ISESSIONACTIONS_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QVector>

class ICourse;
class IPhrase;
class TrainingAction;

/**
 * \class ISessionActions
 *
 * Interface for both training and editor sessions that exposes simple iterator functionalities for a selected course.
 * The interface provides all properties that are needed to create a navigatible menu.
 */
class ARTIKULATECORE_EXPORT ISessionActions : public QObject
{
    Q_OBJECT
public:
    explicit ISessionActions(QObject *parent)
        : QObject(parent)
    {
    }
    ~ISessionActions() override = default;
    virtual TrainingAction *activeAction() const = 0;
    virtual void setActivePhrase(IPhrase *phrase) = 0;
    /**
     * \brief Return tree of training actions
     *
     * The return actions form a 2-level hierarchy:
     * - the first level are all units
     * - the unit actions may contain sub-actions, which are the phrases
     */
    virtual QVector<TrainingAction *> trainingActions() const = 0;

Q_SIGNALS:
    void courseChanged();
    void actionsChanged();
    void phraseChanged();
};
Q_DECLARE_INTERFACE(ISessionActions, "ISessionActions")

#endif
