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



#include "taggroupmodel.h"
#include "core/course.h"
#include "core/unit.h"
#include "core/taggroup.h"

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

TagGroupModel::TagGroupModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_course(0)
    , m_signalMapper(new QSignalMapper(this))
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    setRoleNames(roles);

    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitTagGroupChanged(int)));
}

void TagGroupModel::setCourse(Course *course)
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
        connect(m_course, SIGNAL(tagGroupAboutToBeAdded(TagGroup*,int)), SLOT(onTagGroupAboutToBeAdded(TagGroup*,int)));
        connect(m_course, SIGNAL(tagGroupAdded()), SLOT(onTagGroupAdded()));
        connect(m_course, SIGNAL(tagGroupAboutToBeRemoved(int,int)), SLOT(onTagGroupsAboutToBeRemoved(int,int)));
        connect(m_course, SIGNAL(tagGroupRemoved()), SLOT(onTagGroupsRemoved()));
    }

    endResetModel();

    emit courseChanged();
}

Course * TagGroupModel::course() const
{
    return m_course;
}

QVariant TagGroupModel::data(const QModelIndex& index, int role) const
{
    Q_ASSERT(m_course);

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_course->unitList().count()) {
        return QVariant();
    }

    TagGroup * const tagGroup = m_course->tagGroupList().at(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !tagGroup->title().isEmpty()?
                QVariant(tagGroup->title()): QVariant(i18n("<No title>"));
    case Qt::ToolTipRole:
        return QVariant(i18n("<p>%1</p>", tagGroup->title()));
    case TitleRole:
        return tagGroup->title();
    case IdRole:
        return tagGroup->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(tagGroup);
    default:
        return QVariant();
    }
}

int TagGroupModel::rowCount(const QModelIndex& parent) const
{
    if (!m_course) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }

    return m_course->unitList().count();
}

void TagGroupModel::onTagGroupAboutToBeAdded(TagGroup *tagGroup, int index)
{
    connect(tagGroup, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    //TODO add missing signals
    beginInsertRows(QModelIndex(), index, index);
}

void TagGroupModel::onTagGroupAdded()
{
    updateMappings();
    endInsertRows();
}

void TagGroupModel::onTagGroupsAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void TagGroupModel::onTagGroupsRemoved()
{
    endRemoveRows();
}

void TagGroupModel::emitTagGroupChanged(int row)
{
    emit tagGroupChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant TagGroupModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18n("Title"));
}

void TagGroupModel::updateMappings()
{
    int tagGroups = m_course->tagGroupList().count();
    for (int i = 0; i < tagGroups; i++) {
        m_signalMapper->setMapping(m_course->tagGroupList().at(i), i);
    }
}
