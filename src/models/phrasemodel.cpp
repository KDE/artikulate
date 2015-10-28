/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "core/course.h"
#include "core/unit.h"
#include "core/phrase.h"
#include <QAbstractItemModel>
#include <QSignalMapper>
#include <KLocalizedString>
#include <QDebug>

PhraseModel::PhraseModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_course(nullptr)
    , m_unitSignalMapper(new QSignalMapper)
    , m_phraseSignalMapper(new QSignalMapper)
{
    connect(m_unitSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped),
            this, &PhraseModel::onUnitChanged);
    connect(m_phraseSignalMapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::mapped),
            this, &PhraseModel::onPhraseChanged);
}

QHash< int, QByteArray > PhraseModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[DataRole] = "dataRole";

    return roles;
}

void PhraseModel::setCourse(Course *course)
{
    if (m_course == course) {
        return;
    }

    beginResetModel();

    if (m_course) {
        m_course->disconnect(this);
        foreach (auto unit, m_course->unitList()) {
            unit->disconnect(this);
            foreach (auto phrase, unit->phraseList()) {
                phrase->disconnect(this);
            }
        }
    }

    m_course = course;
    if (m_course) {
        // connect to unit changes
        connect(m_course, &Course::unitAboutToBeAdded, this, &PhraseModel::onUnitAboutToBeAdded);
        connect(m_course, &Course::unitAdded, this, &PhraseModel::onUnitAdded);
        connect(m_course, &Course::unitsAboutToBeRemoved, this, &PhraseModel::onUnitsAboutToBeRemoved);
        connect(m_course, &Course::unitsRemoved, this, &PhraseModel::onUnitsRemoved);

        // initial setting of signal mappings
        foreach (auto unit, m_course->unitList()) {
            // connect to phrase changes
            connect(unit, &Unit::phraseAboutToBeAdded, this, &PhraseModel::onPhraseAboutToBeAdded);
            connect(unit, static_cast<void (Unit::*)()>(&Unit::phraseAdded), this, &PhraseModel::onPhraseAdded);
            connect(unit, &Unit::phraseAboutToBeRemoved, this, &PhraseModel::onPhrasesAboutToBeRemoved);
            connect(unit, static_cast<void (Unit::*)()>(&Unit::phraseRemoved), this, &PhraseModel::onPhrasesRemoved);
            connect(unit, &Unit::titleChanged, m_unitSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));

            // insert and connect all already existing phrases
            int phrases = unit->phraseList().count();
            for (int i = 0; i < phrases; ++i) {
                onPhraseAboutToBeAdded(unit->phraseList().at(i), i);
                endInsertRows();
            }
        }
        updateUnitMappings();
        updatePhraseMappings();
    }

    // emit done
    endResetModel();
    emit courseChanged();
}

Course * PhraseModel::course() const
{
    return m_course;
}

QVariant PhraseModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_course);

    if (!index.isValid()) {
        return QVariant();
    }

    if (!index.internalPointer()) {
        if (!m_course || m_course->unitList().size() == 0) {
            return QVariant();
        }
        Unit *unit = m_course->unitList().at(index.row());
        switch(role)
        {
        case TextRole:
            return unit->title();
        case DataRole:
            return QVariant::fromValue<QObject*>(unit);
        default:
            return QVariant();
        }
    }
    else {
        Unit *unit = static_cast<Unit*>(index.internalPointer());
        switch(role)
        {
        case TextRole:
            return unit->phraseList().at(index.row())->text();
        case DataRole:
            return QVariant::fromValue<QObject*>(unit->phraseList().at(index.row()));
        default:
            return QVariant();
        }
    }
    return QVariant();
}

int PhraseModel::rowCount(const QModelIndex &parent) const
{
    if (!m_course) {
        return 0;
    }

    // no valid index -> must be (invisible) root
    if (!parent.isValid()) {
        return m_course->unitList().count();
    }

    // internal pointer -> must be a phrase
    if (parent.internalPointer()) {
        return 0;
    }

    // else -> must be a unit
    Unit *unit = m_course->unitList().at(parent.row());
    return unit->phraseList().count();
}

int PhraseModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex PhraseModel::parent(const QModelIndex &child) const
{
    if (!child.internalPointer() || !m_course) {
        return QModelIndex();
    }
    Unit *parent = static_cast<Unit*>(child.internalPointer());
    for (int i = 0; i < m_course->unitList().count(); ++i) {
        if (m_course->unitList().at(i) == parent) {
            return createIndex(i, 0);
        }
    }
    return QModelIndex();
}

QModelIndex PhraseModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) { // unit elements
        return createIndex(row, column);
    } else { // phrase elements
        Unit *unit = m_course->unitList().at(parent.row());
        if (unit) {
            return createIndex(row, column, unit);
        }
    }
    return QModelIndex();
}

QModelIndex PhraseModel::indexPhrase(Phrase *phrase) const
{
    if (!phrase) {
        return QModelIndex();
    }
    Unit *unit = phrase->unit();
    return createIndex(unit->phraseList().indexOf(phrase), 0, unit);
}

QModelIndex PhraseModel::indexUnit(Unit *unit) const
{
    if (!unit || !m_course) {
        return QModelIndex();
    }
    return createIndex(m_course->unitList().indexOf(unit), 0);
}

bool PhraseModel::isUnit(const QModelIndex &index) const
{
    return (index.internalPointer() == nullptr);
}

void PhraseModel::onPhraseAboutToBeAdded(Phrase *phrase, int index)
{
    int unitIndex = m_course->unitList().indexOf(phrase->unit());
    connect(phrase, &Phrase::textChanged, m_phraseSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    beginInsertRows(createIndex(unitIndex, 0), index, index);
}

void PhraseModel::onPhraseAdded()
{
    endInsertRows();
    updatePhraseMappings();
}

void PhraseModel::onPhrasesAboutToBeRemoved(int first, int last)
{
    //TODO better solution requires access to unit
    //TODO remove connections from m_phraseSignalMapper
    beginResetModel();
}

void PhraseModel::onPhrasesRemoved()
{
    endResetModel();
}

void PhraseModel::onPhraseChanged(QObject *phrase)
{
    Phrase *changedPhrase = qobject_cast<Phrase*>(phrase);
    Q_ASSERT(changedPhrase);
    QModelIndex index = indexPhrase(changedPhrase);
    emit dataChanged(index, index);
}

void PhraseModel::onUnitAboutToBeAdded(Unit *unit, int index)
{
    Q_UNUSED(unit)
    beginInsertRows(QModelIndex(), index, index);
    connect(unit, &Unit::titleChanged, m_unitSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
}

void PhraseModel::onUnitAdded()
{
    endInsertRows();
    updateUnitMappings();
}

void PhraseModel::onUnitsAboutToBeRemoved(int first, int last)
{
    for (int i = first; i <= last; ++i) {
        Unit *unit = m_course->unitList().at(i);
        disconnect(unit, &Unit::titleChanged, m_unitSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    }
    beginRemoveRows(QModelIndex(), first, last);
}

void PhraseModel::onUnitsRemoved()
{
    endRemoveRows();
}

void PhraseModel::onUnitChanged(int index)
{
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

QVariant PhraseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Phrase"));
}

bool PhraseModel::isPhrase(const QModelIndex &index) const
{
    if (index.internalPointer()) {
        return true;
    }
    return false;
}

Phrase * PhraseModel::phrase(const QModelIndex &index) const
{
    if (index.internalPointer()) {
        Unit *unit = static_cast<Unit *>(index.internalPointer());
        return unit->phraseList().at(index.row());
    }
    if (!m_course->unitList().at(index.row())->phraseList().isEmpty()) {
        return m_course->unitList().at(index.row())->phraseList().first();
    }
    return nullptr;
}

Unit * PhraseModel::unit(const QModelIndex &index) const
{
    return m_course->unitList().at(index.row());
}

void PhraseModel::updateUnitMappings()
{
    int units = m_course->unitList().count();
    for (int i = 0; i < units; ++i) {
        m_unitSignalMapper->setMapping(m_course->unitList().at(i), i);
    }
}

void PhraseModel::updatePhraseMappings()
{
    //TODO this might be quite costly for long units
    // better, implement access based on index pairs
    foreach (const Unit *unit, m_course->unitList()) {
        foreach (Phrase *phrase, unit->phraseList()) {
            m_phraseSignalMapper->setMapping(phrase, phrase);
        }
    }
}
