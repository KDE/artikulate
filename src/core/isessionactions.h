/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
 * Interface for both training and editor sessions that exposes simple iterator functionalities for a selecte course.
 * The interface provides all properties that are needed to create a navigatible menu.
 */
class ARTIKULATECORE_EXPORT ISessionActions : public QObject
{
    Q_OBJECT
public:
    ISessionActions(QObject *parent)
        : QObject(parent)
    {
    }
    virtual ~ISessionActions() = default;
    virtual ICourse * course() const = 0;
    virtual TrainingAction * activeAction() const = 0;
    virtual IPhrase * activePhrase() const = 0;
    virtual void setPhrase(IPhrase *phrase) = 0;
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
    void phraseChanged();
};
Q_DECLARE_INTERFACE(ISessionActions, "ISessionActions")

#endif
