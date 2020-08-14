/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "learningprogressmodel.h"
#include "artikulate_debug.h"
#include "core/trainingsession.h"
#include <KLocalizedString>
#include <QAbstractTableModel>

LearningProgressModel::LearningProgressModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_session(nullptr)
{
}

QHash<int, QByteArray> LearningProgressModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DataRole] = "dataRole";

    return roles;
}

int LearningProgressModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (m_session == 0) {
        return 0;
    }
    // row for every try, plus extra column for 0st and (max + 1)st try
    return m_session->maximumTries() + 2;
}

int LearningProgressModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    // we have 4 different word types
    return 4;
}

TrainingSession *LearningProgressModel::session() const
{
    return m_session;
}

void LearningProgressModel::setSession(TrainingSession *session)
{
    if (m_session == session) {
        return;
    }
    beginResetModel();
    if (m_session) {
        m_session->disconnect(this);
    }
    m_session = session;
    connect(session, SIGNAL(finished()), this, SLOT(updateResults()));
    connect(session, SIGNAL(finished()), this, SIGNAL(maximumTriesChanged()));
    endResetModel();
    emit sessionChanged();
}

int LearningProgressModel::maximumTries() const
{
    if (!m_session) {
        return 0;
    }
    return m_session->maximumTries();
}

int LearningProgressModel::maximumPhrasesPerTry() const
{
    if (!m_session) {
        return 0;
    }
    return m_session->maximumPhrasesPerTry();
}

QVariant LearningProgressModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qCWarning(ARTIKULATE_LOG) << "Invalid index requested";
        return QVariant();
    }

    // column tooltip is number of needed tries
    // first try is 0
    if (role == Qt::ToolTipRole) {
        return QVariant(index.row());
    }

    // otherwise we only support displayrole
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    // handle special rows
    if (index.row() == 0 || index.row() == m_session->maximumTries() + 1) {
        return QVariant(0);
    }

    // normal tries
    int tries = index.row();
    switch (index.column()) {
        case Phrase::Word:
            return QVariant(m_session->numberPhrasesGroupedByTries(TrainingSession::Word, tries));
        case Phrase::Sentence:
            return QVariant(m_session->numberPhrasesGroupedByTries(TrainingSession::Sentence, tries));
        case Phrase::Expression:
            return QVariant(m_session->numberPhrasesGroupedByTries(TrainingSession::Expression, tries));
        case Phrase::Paragraph:
            return QVariant(m_session->numberPhrasesGroupedByTries(TrainingSession::Paragraph, tries));
        default:
            return QVariant();
    }
}

QVariant LearningProgressModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Vertical) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    switch (section) {
        case Phrase::Word:
            return QVariant(i18n("Words"));
        case Phrase::Sentence:
            return QVariant(i18n("Sentences"));
        case Phrase::Expression:
            return QVariant(i18n("Expressions"));
        case Phrase::Paragraph:
            return QVariant(i18n("Paragraphs"));
        default:
            return QVariant();
    }
}

void LearningProgressModel::updateResults()
{
    beginResetModel();
    // nothing to do
    endResetModel();
}
