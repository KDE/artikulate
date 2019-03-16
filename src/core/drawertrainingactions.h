/*
 *  Copyright 2018-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef DRAWERTRAININGACTIONS_H
#define DRAWERTRAININGACTIONS_H

#include "artikulatecore_export.h"
#include "trainingaction.h"
#include <QObject>

class Course;

class ARTIKULATECORE_EXPORT DrawerTrainingActions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TrainingSession *session READ session WRITE setSession NOTIFY sessionChanged)
    Q_PROPERTY(QList<QObject *> actions READ actions NOTIFY actionsChanged)

public:
    DrawerTrainingActions(QObject *parent = nullptr);
    void setSession(TrainingSession *session);
    TrainingSession * session() const;
    QList<QObject *> actions() const;

Q_SIGNALS:
    void actionsChanged();
    void sessionChanged();
    /**
     * Notify that training view shall be displayed.
     */
    void triggerTrainingView();

private:
    TrainingSession *m_session{nullptr};
    TrainingAction *m_defaultAction{nullptr};
};

#endif
