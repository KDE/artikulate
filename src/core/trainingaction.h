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

#ifndef TRAININGACTION_H
#define TRAININGACTION_H

#include "artikulatecore_export.h"
#include "trainingactionicon.h"
#include "iphrase.h"
#include "trainingsession.h"
#include <QObject>
#include <QDebug>

class DrawerTrainingActions;

class ARTIKULATECORE_EXPORT TrainingAction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER m_text CONSTANT)
    Q_PROPERTY(QObject* icon READ icon CONSTANT)
    Q_PROPERTY(bool visible MEMBER m_visible CONSTANT)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool checked READ checked NOTIFY checkedChanged)
    Q_PROPERTY(QString tooltip MEMBER m_tooltip CONSTANT)
    Q_PROPERTY(QList<QObject*> children READ actions NOTIFY actionsChanged)
    Q_PROPERTY(bool checkable MEMBER m_checkable CONSTANT)

public:
    TrainingAction(QObject *parent = nullptr);
    TrainingAction(const QString &text, QObject *parent = nullptr);
    TrainingAction(std::shared_ptr<IPhrase> phrase, ISessionActions *session, QObject *parent = nullptr);
    void appendChild(QObject *child);
    bool hasChildren() const;
    Q_INVOKABLE void trigger();
    bool enabled() const;
    void setEnabled(bool enabled);
    void setChecked(bool checked);
    bool checked() const;
    QObject * icon() const;
    IPhrase * phrase() const;
    QList<QObject*> actions() const;

Q_SIGNALS:
    void changed();
    void actionsChanged();
    void enabledChanged(bool enabled);
    void checkedChanged(bool checked);

private:
    QString m_text;
    TrainingActionIcon *m_icon{nullptr};
    bool m_visible{true};
    bool m_enabled{true};
    bool m_checked{false};
    bool m_checkable{false};
    QString m_tooltip{QString()};
    QList<QObject*> m_actions;
    std::shared_ptr<IPhrase> m_phrase;
    ISessionActions * m_session{nullptr};
};

#endif
