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

#include "drawertrainingactions.h"
#include "trainingaction.h"

#include <KLocalizedString>
#include <QList>
#include <QDebug>

DrawerTrainingActions::DrawerTrainingActions(QObject* parent)
    : QObject{parent}
    , m_defaultAction{new TrainingAction(i18n("Please select a course"), this)}
{
}

void DrawerTrainingActions::setSession(TrainingSession *session)
{
    if (session == m_session) {
        return;
    }
    if (m_session) {
        disconnect(m_session, &TrainingSession::courseChanged, this, &DrawerTrainingActions::actionsChanged);
        disconnect(m_session, &TrainingSession::phraseChanged, this, &DrawerTrainingActions::triggerTrainingView);
    }

    m_session = session;
    connect(m_session, &TrainingSession::courseChanged, this, &DrawerTrainingActions::actionsChanged);
    connect(m_session, &TrainingSession::phraseChanged, this, &DrawerTrainingActions::triggerTrainingView);

    emit sessionChanged();
    emit actionsChanged();
}

TrainingSession * DrawerTrainingActions::session() const
{
    return m_session;
}

QList<QObject *> DrawerTrainingActions::actions() const
{
    if (!m_session || m_session->trainingActions().isEmpty()) {
        QList<QObject *> list;
        list << qobject_cast<QObject*>(m_defaultAction);
        return list;
    }
    QList<QObject *> actions;
    const auto trainingActions = m_session->trainingActions();
    for (const auto &action : qAsConst(trainingActions)) {
        actions.append(qobject_cast<QObject*>(action));
    }
    return actions;
}
