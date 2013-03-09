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



#include "phonemeunitmodel.h"
#include "core/course.h"
#include "core/unit.h"
#include "core/phonemegroup.h"

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

PhonemeUnitModel::PhonemeUnitModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_course(0)
    , m_signalMapper(new QSignalMapper(this))
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[NumberPhrasesRole] = "numberPhrases";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    roles[PhonemeGroupRole] = "phonemeGroupRole";
    setRoleNames(roles);

    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitUnitChanged(int)));
}

void PhonemeUnitModel::setCourse(Course *course)
{
    if (m_course == course) {
        return;
    }

    beginResetModel();

    if (m_course) {
        m_course->disconnect(this);
    }

    m_course = course;

    if (m_course) {
        connect(m_course, SIGNAL(phonemeGroupAboutToBeAdded(PhonemeGroup*,int)), SLOT(onUnitAboutToBeAdded(PhonemeGroup*,int)));
        connect(m_course, SIGNAL(phonemeGroupAdded()), SLOT(onUnitAdded()));
        connect(m_course, SIGNAL(phonemeGroupAboutToBeRemoved(int,int)), SLOT(onUnitsAboutToBeRemoved(int,int)));
        connect(m_course, SIGNAL(phonemeGroupRemoved()), SLOT(onUnitsRemoved()));
    }

    endResetModel();

    emit courseChanged();
}

Course * PhonemeUnitModel::course() const
{
    return m_course;
}

QVariant PhonemeUnitModel::data(const QModelIndex& index, int role) const
{
    Q_ASSERT(m_course);

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_course->phonemeUnitList().count()) {
        return QVariant();
    }

    Unit * const unit = m_course->phonemeUnitList().at(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !unit->title().isEmpty()?
                QVariant(unit->title()): QVariant(i18n("<No title>"));
    case Qt::ToolTipRole:
        return QVariant(i18n("<p>%1</p>", unit->title()));
    case TitleRole:
        return unit->title();
    case NumberPhrasesRole:
        return unit->phraseList(Phrase::AllTypes).count();
    case IdRole:
        return unit->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(unit);
    case PhonemeGroupRole:
        return QVariant::fromValue<QObject*>(m_course->phonemeGroup(unit));
    default:
        return QVariant();
    }
}

int PhonemeUnitModel::rowCount(const QModelIndex& parent) const
{
    if (!m_course) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }

    return m_course->unitList().count();
}

void PhonemeUnitModel::onUnitAboutToBeAdded(PhonemeGroup *phonemeGroup, int index)
{
    connect(phonemeGroup, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    //TODO add missing signals
    beginInsertRows(QModelIndex(), index, index);
}

void PhonemeUnitModel::onUnitAdded()
{
    updateMappings();
    endInsertRows();
}

void PhonemeUnitModel::onUnitsAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void PhonemeUnitModel::onUnitsRemoved()
{
    endRemoveRows();
}

void PhonemeUnitModel::emitUnitChanged(int row)
{
    emit unitChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant PhonemeUnitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18n("Title"));
}

void PhonemeUnitModel::updateMappings()
{
    int units = m_course->unitList().count();
    for (int i = 0; i < units; i++) {
        m_signalMapper->setMapping(m_course->unitList().at(i), i);
    }
}
