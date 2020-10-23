/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "phonemeunitmodel.h"
#include "core/icourse.h"
#include "core/phonemegroup.h"
#include "core/unit.h"

#include <QSignalMapper>

#include "artikulate_debug.h"
#include <KLocalizedString>

PhonemeUnitModel::PhonemeUnitModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_course(nullptr)
    , m_phonemeGroup(nullptr)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitUnitChanged(int)));
    connect(this, &PhonemeUnitModel::phonemeGroupChanged, this, &PhonemeUnitModel::countChanged);
    connect(this, &PhonemeUnitModel::courseChanged, this, &PhonemeUnitModel::countChanged);
}

QHash<int, QByteArray> PhonemeUnitModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[NumberPhrasesRole] = "numberPhrases";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    roles[PhonemeGroupRole] = "phonemeGroupRole";

    return roles;
}

void PhonemeUnitModel::setCourse(ICourse *course)
{
    if (m_course == course) {
        return;
    }

    beginResetModel();

    if (m_course) {
        m_course->disconnect(this);
    }

    m_course = course;

    // TODO currently no phonems supported in icourse
    //    if (m_course) {
    //        connect(m_course, &Course::phonemeGroupAboutToBeAdded, this, &PhonemeUnitModel::onUnitAboutToBeAdded);
    //        connect(m_course, &Course::phonemeGroupAdded, this, &PhonemeUnitModel::onUnitAdded);
    //        connect(m_course, &Course::phonemeGroupAboutToBeRemoved, this, &PhonemeUnitModel::onUnitsAboutToBeRemoved);
    //        connect(m_course, &Course::phonemeGroupRemoved, this, &PhonemeUnitModel::onUnitsRemoved);
    //    }

    endResetModel();

    emit courseChanged();
}

void PhonemeUnitModel::setPhonemeGroup(PhonemeGroup *phonemeGroup)
{
    if (m_phonemeGroup == phonemeGroup) {
        return;
    }
    beginResetModel();
    m_phonemeGroup = phonemeGroup;
    endResetModel();

    emit phonemeGroupChanged();
}

PhonemeGroup *PhonemeUnitModel::phonemeGroup() const
{
    return m_phonemeGroup;
}

ICourse *PhonemeUnitModel::course() const
{
    return m_course;
}

QVariant PhonemeUnitModel::data(const QModelIndex &index, int role) const
{
    // TODO currently no phonems supported
    return QVariant();
    //    Q_ASSERT(m_course);
    //    Q_ASSERT(m_phonemeGroup);

    //    if (!index.isValid()) {
    //        return QVariant();
    //    }

    //    if (index.row() >= m_course->phonemeUnitList(m_phonemeGroup).count()) {
    //        return QVariant();
    //    }

    //    Unit * const unit = m_course->phonemeUnitList(m_phonemeGroup).at(index.row());

    //    switch(role)
    //    {
    //    case Qt::DisplayRole:
    //        return !unit->title().isEmpty()?
    //                QVariant(unit->title()): QVariant(i18nc("@item:inlistbox:", "unknown"));
    //    case Qt::ToolTipRole:
    //        return QVariant(unit->title());
    //    case TitleRole:
    //        return unit->title();
    //    case NumberPhrasesRole:
    //        return unit->phraseList().count();
    //    case IdRole:
    //        return unit->id();
    //    case DataRole:
    //        return QVariant::fromValue<QObject*>(unit);
    //    case PhonemeGroupRole:
    //        return QVariant::fromValue<QObject*>(m_course->phonemeGroup(unit));
    //    default:
    //        return QVariant();
    //    }
}

int PhonemeUnitModel::rowCount(const QModelIndex &parent) const
{
    // TODO currently no phonems supported in icourse
    return 0;
    //    if (!m_course) {
    //        return 0;
    //    }

    //    if (parent.isValid()) {
    //        return 0;
    //    }

    //    return m_course->phonemeUnitList(m_phonemeGroup).count();
}

void PhonemeUnitModel::onUnitAboutToBeAdded(PhonemeGroup *phonemeGroup, int index)
{
    connect(phonemeGroup, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    // TODO add missing signals
    beginInsertRows(QModelIndex(), index, index);
}

void PhonemeUnitModel::onUnitAdded()
{
    updateMappings();
    endInsertRows();
    emit countChanged();
}

void PhonemeUnitModel::onUnitsAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void PhonemeUnitModel::onUnitsRemoved()
{
    endRemoveRows();
    emit countChanged();
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
    return QVariant(i18nc("@title:column", "Phoneme Unit"));
}

int PhonemeUnitModel::count() const
{
    // TODO currently no phonems supported
    return 0;
    //    if (!m_course || !m_phonemeGroup) {
    //        return 0;
    //    }
    //    return m_course->phonemeUnitList(m_phonemeGroup).count();
}

void PhonemeUnitModel::updateMappings()
{
    //    int units = m_course->phonemeUnitList(m_phonemeGroup).count();
    //    for (int i = 0; i < units; i++) {
    //        m_signalMapper->setMapping(m_course->phonemeUnitList(m_phonemeGroup).at(i), i);
    //    }
}
