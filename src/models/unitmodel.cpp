/*
    SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "unitmodel.h"
#include "core/icourse.h"
#include "core/language.h"
#include "core/phrase.h"
#include "core/unit.h"

#include <QSignalMapper>

#include "artikulate_debug.h"
#include <KLocalizedString>

UnitModel::UnitModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_course(nullptr)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitUnitChanged(int)));
}

QHash<int, QByteArray> UnitModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[ContainsTrainingData] = "containsTrainingData";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";

    return roles;
}

void UnitModel::setCourse(ICourse *course)
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
        connect(m_course, &ICourse::unitAboutToBeAdded, this, &UnitModel::onUnitAboutToBeAdded);
        connect(m_course, &ICourse::unitAdded, this, &UnitModel::onUnitAdded);
        connect(m_course, &ICourse::unitsAboutToBeRemoved, this, &UnitModel::onUnitsAboutToBeRemoved);
        connect(m_course, &ICourse::unitsRemoved, this, &UnitModel::onUnitsRemoved);
    }

    endResetModel();

    emit courseChanged();
}

ICourse *UnitModel::course() const
{
    return m_course;
}

QVariant UnitModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_course);

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_course->units().count()) {
        return QVariant();
    }

    auto unit = m_course->units().at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return !unit->title().isEmpty() ? QVariant(unit->title()) : QVariant(i18nc("@item:inlistbox:", "unknown"));
        case Qt::ToolTipRole:
            return QVariant(unit->title());
        case TitleRole:
            return unit->title();
        case ContainsTrainingData:
            for (const auto &phrase : unit->phrases()) {
                //            if (phrase->editState() == Phrase::Completed) { //TODO introduce editablephrase
                //                return true;
                //            }
            }
            return false;
        case IdRole:
            return unit->id();
        case DataRole:
            return QVariant::fromValue<QObject *>(unit.get());
        default:
            return QVariant();
    }
}

int UnitModel::rowCount(const QModelIndex &parent) const
{
    if (!m_course) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }

    return m_course->units().count();
}

void UnitModel::onUnitAboutToBeAdded(std::shared_ptr<Unit> unit, int index)
{
    connect(unit.get(), SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    // TODO add missing signals
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
    return QVariant(i18nc("@title:column", "Unit"));
}

void UnitModel::updateMappings()
{
    int units = m_course->units().count();
    for (int i = 0; i < units; i++) {
        m_signalMapper->setMapping(m_course->units().at(i).get(), i);
    }
}
