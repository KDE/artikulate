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

#include "phraselistmodel.h"
#include "core/unit.h"
#include "core/phrase.h"
#include <QAbstractListModel>
#include <QSignalMapper>
#include <KLocalizedString>

PhraseListModel::PhraseListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_unit(nullptr)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitPhraseChanged(int)));

    // connect all phrase number operations to single signal
    connect(this, &PhraseListModel::typeChanged, this, &PhraseListModel::countChanged);
    connect(this, &PhraseListModel::unitChanged, this, &PhraseListModel::countChanged);
}

QHash< int, QByteArray > PhraseListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[SoundFileRole] = "soundFile";
    roles[IdRole] = "id";
    roles[TypeRole] = "type";
    roles[ExcludedRole] = "excludedRole";
    roles[DataRole] = "dataRole";

    return roles;
}

void PhraseListModel::setUnit(Unit *unit)
{
    if (m_unit == unit) {
        return;
    }

    beginResetModel();

    if (m_unit) {
        m_unit->disconnect(this);
        foreach (Phrase *phrase, m_unit->phraseList()) {
            phrase->disconnect(this);
        }
    }

    m_unit = unit;
    if (m_unit) {
        // initial setting of signal mappings
        connect(m_unit, &Unit::phraseAboutToBeAdded, this, &PhraseListModel::onPhraseAboutToBeAdded);
        connect(m_unit, &Unit::phraseAdded, this, &PhraseListModel::onPhraseAdded);
        connect(m_unit, &Unit::phraseAboutToBeRemoved, this, &PhraseListModel::onPhrasesAboutToBeRemoved);
        connect(m_unit, &Unit::phraseRemoved, this, &PhraseListModel::onPhrasesRemoved);

        // insert and connect all already existing phrases
        int phrases = m_unit->phraseList().count();
        for (int i=0; i < phrases; ++i) {
            onPhraseAboutToBeAdded(m_unit->phraseList().at(i), i);
            endInsertRows();
            emit countChanged();
        }
        updateMappings();
    }

    // emit done
    endResetModel();
    emit unitChanged();
}

Unit * PhraseListModel::unit() const
{
    return m_unit;
}

QVariant PhraseListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_unit);

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_unit->phraseList().count()) {
        return QVariant();
    }

    Phrase * const phrase = m_unit->phraseList().at(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !phrase->text().isEmpty()?
                QVariant(phrase->text()): QVariant(i18nc("@item:inlistbox:", "unknown"));
    case Qt::ToolTipRole:
        return QVariant(phrase->text());
    case TextRole:
        return phrase->text();
    case SoundFileRole:
        return phrase->sound();
    case IdRole:
        return phrase->id();
    case TypeRole:
        return phrase->type();
    case ExcludedRole:
        return phrase->isExcluded();
    case DataRole:
        return QVariant::fromValue<QObject*>(phrase);
    default:
        return QVariant();
    }
}

int PhraseListModel::rowCount(const QModelIndex &parent) const
{
    if (!m_unit) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }
    return m_unit->phraseList().count();
}

void PhraseListModel::onPhraseAboutToBeAdded(Phrase *phrase, int index)
{
    connect(phrase, SIGNAL(textChanged()), m_signalMapper, SLOT(map()));
    connect(phrase, SIGNAL(typeChanged()), m_signalMapper, SLOT(map()));
    connect(phrase, SIGNAL(excludedChanged()), m_signalMapper, SLOT(map()));
    beginInsertRows(QModelIndex(), index, index);
}

void PhraseListModel::onPhraseAdded()
{
    updateMappings();
    endInsertRows();
    emit countChanged();
}

void PhraseListModel::onPhrasesAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void PhraseListModel::onPhrasesRemoved()
{
    endRemoveRows();
    emit countChanged();
}

void PhraseListModel::emitPhraseChanged(int row)
{
    beginResetModel();
    endResetModel();
        //FIXME very inefficient, but workaround to force new filtering in phrasefiltermodel
        //      to exclude possible new excluded phrases
    emit phraseChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant PhraseListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Phrase"));
}

int PhraseListModel::count() const
{
    if (!m_unit) {
        return 0;
    }
    return m_unit->phraseList().count();
}

void PhraseListModel::updateMappings()
{
    if (!m_unit) {
        return;
    }
    int phrases = m_unit->phraseList().count();
    for (int i = 0; i < phrases; ++i) {
        m_signalMapper->setMapping(m_unit->phraseList().at(i), i);
    }
}
