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
#include "iphrase.h"
#include "trainingactionicon.h"
#include "trainingsession.h"
#include <QDebug>
#include <QAbstractItemModel>
#include <QObject>

class DrawerTrainingActions;

class ARTIKULATECORE_EXPORT TrainingAction : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString title READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QObject *icon READ icon CONSTANT)
    Q_PROPERTY(bool visible MEMBER m_visible CONSTANT)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool checked READ checked NOTIFY checkedChanged)
    Q_PROPERTY(QString tooltip MEMBER m_tooltip CONSTANT)
    Q_PROPERTY(QAbstractItemModel * children READ actionModel NOTIFY actionsChanged)
    Q_PROPERTY(bool checkable MEMBER m_checkable CONSTANT)
    Q_PROPERTY(int length READ actionsCount NOTIFY actionsChanged)

public:
    enum ModelRoles {
        ModelDataRole = Qt::UserRole + 1
    };

    TrainingAction(QObject *parent = nullptr);
    TrainingAction(const QString &text, QObject *parent = nullptr);
    TrainingAction(std::shared_ptr<IPhrase> phrase, ISessionActions *session, QObject *parent = nullptr);
    Q_INVOKABLE void trigger();
    bool enabled() const;
    void setEnabled(bool enabled);
    QString text() const;
    void setText(QString text);
    void setChecked(bool checked);
    bool checked() const;
    QObject *icon();
    IPhrase *phrase() const;
    QAbstractListModel * actionModel();
    QVector<TrainingAction*> actions() const;
    int actionsCount() const;
    bool hasActions() { return m_actions.count() > 0; }
    TrainingAction * action(int index) const;
    void appendAction(TrainingAction *action);
    void clearActions();
    QHash<int, QByteArray> roleNames() const override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void actionsChanged();
    void enabledChanged(bool enabled);
    void checkedChanged(bool checked);
    void textChanged(QString text);

private:
    QVector<TrainingAction *> m_actions;
    QString m_text;
    TrainingActionIcon m_icon;
    bool m_visible {true};
    bool m_enabled {true};
    bool m_checked {false};
    bool m_checkable {false};
    QString m_tooltip {QString()};
    std::shared_ptr<IPhrase> m_phrase;
    ISessionActions *m_session {nullptr};
};

#endif
