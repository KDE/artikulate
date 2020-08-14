/*
 *  SPDX-FileCopyrightText: 2018-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TRAININGACTION_H
#define TRAININGACTION_H

#include "artikulatecore_export.h"
#include "iphrase.h"
#include "trainingactionicon.h"
#include "trainingsession.h"
#include <QAbstractItemModel>
#include <QDebug>
#include <QObject>

class DrawerTrainingActions;

class ARTIKULATECORE_EXPORT TrainingAction : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString title READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QObject *icon READ icon CONSTANT)
    Q_PROPERTY(QObject *parent READ parent WRITE setParent CONSTANT)
    Q_PROPERTY(bool visible MEMBER m_visible CONSTANT)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool checked READ checked NOTIFY checkedChanged)
    Q_PROPERTY(QString tooltip MEMBER m_tooltip CONSTANT)
    Q_PROPERTY(QAbstractItemModel *children READ actionModel NOTIFY actionsChanged)
    Q_PROPERTY(bool checkable MEMBER m_checkable CONSTANT)
    Q_PROPERTY(int length READ actionsCount NOTIFY actionsChanged)

public:
    enum ModelRoles { ModelDataRole = Qt::UserRole + 1 };

    explicit TrainingAction(QObject *parent = nullptr);
    explicit TrainingAction(const QString &text, QObject *parent = nullptr);
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
    QAbstractListModel *actionModel();
    QVector<TrainingAction *> actions() const;
    int actionsCount() const;
    bool hasActions()
    {
        return m_actions.count() > 0;
    }
    TrainingAction *action(int index) const;
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
