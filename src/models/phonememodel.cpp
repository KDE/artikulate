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

#include "phonememodel.h"
#include "core/language.h"
#include "core/phoneme.h"

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

PhonemeModel::PhonemeModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_language(0)
    , m_signalMapper(new QSignalMapper(this))
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    setRoleNames(roles);

    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitPhonemeChanged(int)));
}

Language * PhonemeModel::language() const
{
    return m_language;
}

void PhonemeModel::setLanguage(Language *language)
{
    emit beginResetModel();

    m_language = language;
    emit languageChanged();

    emit endResetModel();
}

QVariant PhonemeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_language->phonemes().count()) {
        return QVariant();
    }

    Phoneme * const phoneme = m_language->phonemes().at(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !phoneme->title().isEmpty()?
                QVariant(phoneme->title()): QVariant(i18nc("@item:inlistbox:", "unknown"));
    case Qt::ToolTipRole:
        return QVariant(phoneme->title());
    case TitleRole:
        return phoneme->title();
    case IdRole:
        return phoneme->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(phoneme);
    default:
        return QVariant();
    }
}

int PhonemeModel::rowCount(const QModelIndex& parent) const
{
    if (!m_language) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }
    return m_language->phonemes().count();
}

void PhonemeModel::onPhonemeAboutToBeAdded(Phoneme *phoneme, int index)
{
    connect(phoneme, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    beginInsertRows(QModelIndex(), index, index);
}

void PhonemeModel::onPhonemeAdded()
{
    updateMappings();
    endInsertRows();
}

void PhonemeModel::onPhonemesAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void PhonemeModel::onPhonemesRemoved()
{
    endRemoveRows();
}

void PhonemeModel::emitPhonemeChanged(int row)
{
    emit phonemeChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant PhonemeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Phoneme"));
}

void PhonemeModel::updateMappings()
{
    if (!m_language) {
        kDebug() << "Aborting to update mappings, language not set.";
        return;
    }
    int phonemes = m_language->phonemes().count();
    for (int i = 0; i < phonemes; i++) {
        m_signalMapper->setMapping(m_language->phonemes().at(i), i);
    }
}
