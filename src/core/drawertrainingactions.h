/*
 *  SPDX-FileCopyrightText: 2018-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef DRAWERTRAININGACTIONS_H
#define DRAWERTRAININGACTIONS_H

#include "artikulatecore_export.h"
#include "isessionactions.h"
#include <QObject>

class Course;

class ARTIKULATECORE_EXPORT DrawerTrainingActions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ISessionActions *session READ session WRITE setSession NOTIFY sessionChanged)
    Q_PROPERTY(QList<QObject *> actions READ actions NOTIFY actionsChanged)

public:
    explicit DrawerTrainingActions(QObject *parent = nullptr);
    void setSession(ISessionActions *session);
    ISessionActions *session() const;
    QList<QObject *> actions() const;

Q_SIGNALS:
    void actionsChanged();
    void sessionChanged();
    /**
     * Notify that course view shall be displayed.
     */
    void triggerPhraseView();

private:
    ISessionActions *m_session {nullptr};
    TrainingAction *m_defaultAction {nullptr};
};

#endif
