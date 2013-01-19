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



#include "unitmodel.h"
#include "core/course.h"
#include "core/unit.h"

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

UnitModel::UnitModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_course(0)
    , m_signalMapper(new QSignalMapper(this))
{
    kDebug() << "create unit model";

    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[NumberPhrasesRole] = "numberPhrases";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    setRoleNames(roles);

    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitUnitChanged(int)));
}

void UnitModel::setCourse(Course *course)
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
        // TODO currently it is not expected that units are added at runtime
        // this may change for the future and should be implemented
    }

    endResetModel();

    emit courseChanged();
}

Course * UnitModel::course() const
{
    return m_course;
}

QVariant UnitModel::data(const QModelIndex& index, int role) const
{
    Q_ASSERT(m_course);

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_course->unitList().count()) {
        return QVariant();
    }

    Unit * const unit = m_course->unitList().at(index.row());

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
        return unit->phraseList().count();
    case IdRole:
        return unit->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(unit);
    default:
        return QVariant();
    }
}

int UnitModel::rowCount(const QModelIndex& parent) const
{
    if (!m_course) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }

    return m_course->unitList().count();
}

void UnitModel::onUnitAboutToBeAdded(Unit *unit, int index)
{
    connect(unit, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    //TODO add missing signals
    beginInsertRows(QModelIndex(), index, index);
}

void UnitModel::onUnitAdded()
{
    updateMappings();
    endInsertRows();
}

void UnitModel::onUnitsAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void UnitModel::onUnitsRemoved()
{
    endRemoveRows();
}

void UnitModel::emitUnitChanged(int row)
{
    emit unitChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant UnitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18n("Title"));
}

void UnitModel::updateMappings()
{
    int units = m_course->unitList().count();
    for (int i = 0; i < units; i++) {
        m_signalMapper->setMapping(m_course->unitList().at(i), i);
    }
}
