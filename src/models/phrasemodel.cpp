/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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



#include "phrasemodel.h"
#include "core/unit.h"
#include "core/phrase.h"

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

PhraseModel::PhraseModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_unit(0)
    , m_signalMapper(new QSignalMapper(this))
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[SoundFileRole] = "soundFile";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    setRoleNames(roles);

    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitPhraseChanged(int)));

    // connect all phrase number operations to single signal
    connect(this, SIGNAL(typeChanged()), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(unitChanged()), this, SIGNAL(countChanged()));
}

void PhraseModel::setUnit(Unit *unit)
{
    if (m_unit == unit) {
        return;
    }

    beginResetModel();

    if (m_unit) {
        m_unit->disconnect(this);
    }

    m_unit = unit;

    if (m_unit) {
        connect(m_unit, SIGNAL(phraseAboutToBeAdded(Phrase*,int)), SLOT(onPhraseAboutToBeAdded(Phrase*,int)));
        connect(m_unit, SIGNAL(phraseAdded()), SLOT(onPhraseAdded()));
        connect(m_unit, SIGNAL(phraseAboutToBeRemoved(int,int)), SLOT(onPhrasesAboutToBeRemoved(int,int)));
        connect(m_unit, SIGNAL(phraseRemoved()), SLOT(onPhrasesRemoved()));
    }

    endResetModel();

    emit unitChanged();
}

Unit * PhraseModel::unit() const
{
    return m_unit;
}

QVariant PhraseModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_unit);

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_unit->phraseList(m_type).count()) {
        return QVariant();
    }

    Phrase * const phrase = m_unit->phraseList(m_type).at(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !phrase->text().isEmpty()?
                QVariant(phrase->text()): QVariant(i18n("<No title>"));
    case Qt::ToolTipRole:
        return QVariant(i18n("<p>%1</p>", phrase->text()));
    case TextRole:
        return phrase->text();
    case SoundFileRole:
        return phrase->sound();
    case IdRole:
        return phrase->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(phrase);
    default:
        return QVariant();
    }
}

void PhraseModel::setType(Phrase::Type type)
{
    if (m_type == type) {
        return;
    }
    beginResetModel();
    m_type = type;
    endResetModel();

    emit typeChanged();
}

Phrase::Type PhraseModel::type() const
{
    return m_type;
}

int PhraseModel::rowCount(const QModelIndex &parent) const
{
    if (!m_unit) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }
    return m_unit->phraseList(m_type).count();
}

void PhraseModel::onPhraseAboutToBeAdded(Phrase *phrase, int index)
{
    // only connect the phrase if the type is correct
    // if the type is not the selected type this->m_type, all following operations will have no impact
    if (phrase->type() != m_type) {
        beginInsertRows(QModelIndex(), index, index);
        return;
    }
    connect(phrase, SIGNAL(textChanged()), m_signalMapper, SLOT(map()));
    beginInsertRows(QModelIndex(), index, index);
}

void PhraseModel::onPhraseAdded()
{
    updateMappings();
    endInsertRows();
    emit countChanged();
}

void PhraseModel::onPhrasesAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void PhraseModel::onPhrasesRemoved()
{
    endRemoveRows();
    emit countChanged();
}

void PhraseModel::emitPhraseChanged(int row)
{
    emit phraseChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant PhraseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18n("Title"));
}

int PhraseModel::count() const
{
    if (!m_unit) {
        return 0;
    }
    return m_unit->phraseList(m_type).count();
}

void PhraseModel::updateMappings()
{
    int phrases = m_unit->phraseList(m_type).count();
    for (int i = 0; i < phrases; i++) {
        m_signalMapper->setMapping(m_unit->phraseList(m_type).at(i), i);
    }
}
