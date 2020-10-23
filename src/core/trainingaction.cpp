/*
 *  SPDX-FileCopyrightText: 2018-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "trainingaction.h"
#include "drawertrainingactions.h"
#include <QQmlEngine>

TrainingAction::TrainingAction(QObject *parent)
    : QAbstractListModel(parent)
    , m_text(QString())
    , m_icon(nullptr, QString()) // TODO "rating-unrated" vs. "rating"
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

TrainingAction::TrainingAction(const QString &text, QObject *parent)
    : QAbstractListModel(parent)
    , m_text(text)
    , m_icon(nullptr, QString()) // TODO "rating-unrated" vs. "rating"
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

TrainingAction::TrainingAction(std::shared_ptr<IPhrase> phrase, ISessionActions *session, QObject *parent)
    : QAbstractListModel(parent)
    , m_icon(nullptr, QString())
    , m_phrase(phrase)
    , m_session(session)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    if (m_phrase) {
        m_text = phrase->text();
    }
    connect(phrase.get(), &IPhrase::textChanged, this, [=]() {
        m_text = phrase->text();
        emit textChanged(m_text);
    });
}

QHash<int, QByteArray> TrainingAction::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ModelDataRole] = "modelData";
    return roles;
}

int TrainingAction::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 1;
}

int TrainingAction::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return actionsCount();
}

QVariant TrainingAction::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= actionsCount()) {
        return QVariant();
    }

    switch (role) {
        case ModelDataRole:
            return QVariant::fromValue<QObject *>(m_actions.at(index.row()));
        case Qt::DisplayRole:
            return m_text;
        default:
            return QVariant();
    }
}

void TrainingAction::trigger()
{
    if (m_phrase && m_session) {
        m_session->setActivePhrase(m_phrase.get());
    }
}

bool TrainingAction::enabled() const
{
    return m_enabled;
}

void TrainingAction::setEnabled(bool enabled)
{
    if (enabled == m_enabled) {
        return;
    }
    m_enabled = enabled;
    emit enabledChanged(m_enabled);
}

QString TrainingAction::text() const
{
    return m_text;
}

void TrainingAction::setText(QString text)
{
    if (text == m_text) {
        return;
    }
    m_text = std::move(text);
    emit textChanged(m_text);
}

bool TrainingAction::checked() const
{
    return m_checked;
}

void TrainingAction::setChecked(bool checked)
{
    if (checked == m_checked) {
        return;
    }
    m_checked = checked;
    emit checkedChanged(m_checked);
}

QObject *TrainingAction::icon()
{
    return qobject_cast<QObject *>(&m_icon);
}

IPhrase *TrainingAction::phrase() const
{
    return m_phrase.get();
}

QVector<TrainingAction *> TrainingAction::actions() const
{
    return m_actions;
}

QAbstractListModel *TrainingAction::actionModel()
{
    return this;
}

int TrainingAction::actionsCount() const
{
    return m_actions.count();
}

TrainingAction *TrainingAction::action(int index) const
{
    if (index < 0 || index >= m_actions.count()) {
        qWarning() << "index not in range, aborting";
        return nullptr;
    }
    return m_actions.at(index);
}

void TrainingAction::appendAction(TrainingAction *action)
{
    beginInsertRows(QModelIndex(), m_actions.count(), m_actions.count());
    m_actions.append(action);
    endInsertRows();
    emit actionsChanged();
}

void TrainingAction::clearActions()
{
    beginResetModel();
    m_actions.clear();
    qDeleteAll(m_actions);
    endResetModel();
    emit actionsChanged();
}
