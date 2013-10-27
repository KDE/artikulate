/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "learningprogressmodel.h"
#include "core/trainingsession.h"
#include <QAbstractTableModel>
#include <KLocale>

LearningProgressModel::LearningProgressModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_session(0)
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DataRole] = "dataRole";
    setRoleNames(roles);
}

int LearningProgressModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid() || m_session == 0) {
        return 0;
    }
    return m_session->maximumTries();
}

int LearningProgressModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 0;
    }
    // we have 4 word types
    return 4;
}

TrainingSession * LearningProgressModel::session() const
{
    return m_session;
}

void LearningProgressModel::setSession(TrainingSession* session)
{
    if (m_session == session) {
        return;
    }
    m_session = session;
    emit sessionChanged();
}

QVariant LearningProgressModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.column() > columnCount() || index.column() < 0) {
        return QVariant();
    }
    if (index.row() > rowCount() || index.row() < 0) {
        return QVariant();
    }
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    int tries = index.row();
    switch (index.column())
    {
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

    switch (section)
    {
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
