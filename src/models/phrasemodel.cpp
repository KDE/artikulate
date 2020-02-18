/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "phrasemodel.h"
#include "artikulate_debug.h"
#include "core/icourse.h"
#include "core/phrase.h"
#include "core/unit.h"
#include <KLocalizedString>
#include <QAbstractItemModel>
#include <QSignalMapper>

PhraseModel::PhraseModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_course(nullptr)
    , m_unitSignalMapper(new QSignalMapper)
    , m_phraseSignalMapper(new QSignalMapper)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(m_unitSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &PhraseModel::onUnitChanged);
#else
    connect(m_unitSignalMapper, &QSignalMapper::mappedInt, this, &PhraseModel::onUnitChanged);
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(m_phraseSignalMapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::mapped), this, &PhraseModel::onPhraseChanged);
#else
    connect(m_phraseSignalMapper, &QSignalMapper::mappedObject, this, &PhraseModel::onPhraseChanged);
#endif
}

QHash<int, QByteArray> PhraseModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[DataRole] = "dataRole";

    return roles;
}

void PhraseModel::setCourse(ICourse *course)
{
    if (m_course == course) {
        return;
    }

    beginResetModel();

    if (m_course) {
        m_course->disconnect(this);
        for (auto unit : m_course->units()) {
            unit->disconnect(this);
            for (auto &phrase : unit->phrases()) {
                phrase->disconnect(this);
            }
        }
    }

    m_course = course;
    if (m_course) {
        // connect to unit changes
        connect(m_course, &ICourse::unitAboutToBeAdded, this, &PhraseModel::onUnitAboutToBeAdded);
        connect(m_course, &ICourse::unitAdded, this, &PhraseModel::onUnitAdded);
        connect(m_course, &ICourse::unitsAboutToBeRemoved, this, &PhraseModel::onUnitsAboutToBeRemoved);
        connect(m_course, &ICourse::unitsRemoved, this, &PhraseModel::onUnitsRemoved);

        // initial setting of signal mappings
        for (auto unit : m_course->units()) {
            // connect to phrase changes
            connect(unit.get(), &Unit::phraseAboutToBeAdded, this, &PhraseModel::onPhraseAboutToBeAdded);
            connect(unit.get(), &Unit::phraseAdded, this, &PhraseModel::onPhraseAdded);
            connect(unit.get(), &Unit::phraseAboutToBeRemoved, this, &PhraseModel::onPhraseAboutToBeRemoved);
            connect(unit.get(), &Unit::phraseRemoved, this, &PhraseModel::onPhrasesRemoved);
            connect(unit.get(), &Unit::titleChanged, m_unitSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));

            // insert and connect all already existing phrases
            int phrases = unit->phrases().count();
            for (int i = 0; i < phrases; ++i) {
                onPhraseAboutToBeAdded(unit->phrases().at(i), i);
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

ICourse *PhraseModel::course() const
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
        if (!m_course || m_course->units().size() == 0) {
            return QVariant();
        }
        auto unit = m_course->units().at(index.row());
        switch (role) {
            case TextRole:
                return unit->title();
            case DataRole:
                return QVariant::fromValue<QObject *>(unit.get());
            default:
                return QVariant();
        }
    } else {
        Unit *unit = static_cast<Unit *>(index.internalPointer());
        switch (role) {
            case TextRole:
                return unit->phrases().at(index.row())->text();
            case DataRole:
                return QVariant::fromValue<QObject *>(unit->phrases().at(index.row()).get());
            default:
                return QVariant();
        }
    }
}

int PhraseModel::rowCount(const QModelIndex &parent) const
{
    if (!m_course) {
        return 0;
    }

    // no valid index -> must be (invisible) root
    if (!parent.isValid()) {
        return m_course->units().count();
    }

    // internal pointer -> must be a phrase
    if (parent.internalPointer()) {
        return 0;
    }

    // else -> must be a unit
    Unit *unit = m_course->units().at(parent.row()).get();
    return unit->phrases().count();
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
    Unit *parent = static_cast<Unit *>(child.internalPointer());
    for (int i = 0; i < m_course->units().count(); ++i) {
        if (m_course->units().at(i).get() == parent) {
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
        auto unit = m_course->units().at(parent.row());
        if (unit) {
            return createIndex(row, column, unit.get());
        }
    }
    return QModelIndex();
}

QModelIndex PhraseModel::indexPhrase(Phrase *phrase) const
{
    if (!phrase) {
        return QModelIndex();
    }
    auto unit = phrase->unit();
    return createIndex(unit->phrases().indexOf(phrase->self()), 0, unit.get());
}

QModelIndex PhraseModel::indexUnit(Unit *unit) const
{
    if (!unit || !m_course) {
        return QModelIndex();
    }
    int uIndex {-1};
    for (int i = 0; i < m_course->units().size(); ++i) {
        if (m_course->units().at(i)->id() == unit->id()) {
            uIndex = i;
            break;
        }
    }
    return createIndex(uIndex, 0);
}

bool PhraseModel::isUnit(const QModelIndex &index) const
{
    return (index.internalPointer() == nullptr);
}

void PhraseModel::onPhraseAboutToBeAdded(std::shared_ptr<IPhrase> phrase, int index)
{
    int uIndex {-1};
    for (int i = 0; i < m_course->units().size(); ++i) {
        if (m_course->units().at(i)->id() == phrase->unit()->id()) {
            uIndex = i;
            break;
        }
    }
    connect(phrase.get(), &IPhrase::textChanged, m_phraseSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    beginInsertRows(createIndex(uIndex, 0), index, index);
}

void PhraseModel::onPhraseAdded()
{
    endInsertRows();
    updatePhraseMappings();
}

void PhraseModel::onPhraseAboutToBeRemoved(int index)
{
    Q_UNUSED(index)
    // TODO better solution requires access to unit
    // TODO remove connections from m_phraseSignalMapper
    beginResetModel();
}

void PhraseModel::onPhrasesRemoved()
{
    endResetModel();
}

void PhraseModel::onPhraseChanged(QObject *phrase)
{
    Phrase *changedPhrase = qobject_cast<Phrase *>(phrase);
    Q_ASSERT(changedPhrase);
    QModelIndex index = indexPhrase(changedPhrase);
    emit dataChanged(index, index);
}

void PhraseModel::onUnitAboutToBeAdded(std::shared_ptr<Unit> unit, int index)
{
    Q_UNUSED(unit)
    beginInsertRows(QModelIndex(), index, index);
    connect(unit.get(), &Unit::titleChanged, m_unitSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
}

void PhraseModel::onUnitAdded()
{
    endInsertRows();
    updateUnitMappings();
}

void PhraseModel::onUnitsAboutToBeRemoved(int first, int last)
{
    for (int i = first; i <= last; ++i) {
        auto unit = m_course->units().at(i);
        disconnect(unit.get(), &Unit::titleChanged, m_unitSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
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

IPhrase *PhraseModel::phrase(const QModelIndex &index) const
{
    if (index.internalPointer()) {
        Unit *unit = static_cast<Unit *>(index.internalPointer());
        return unit->phrases().at(index.row()).get();
    }
    if (!m_course->units().at(index.row())->phrases().isEmpty()) {
        return m_course->units().at(index.row())->phrases().first().get();
    }
    return nullptr;
}

Unit *PhraseModel::unit(const QModelIndex &index) const
{
    return m_course->units().at(index.row()).get();
}

void PhraseModel::updateUnitMappings()
{
    int units = m_course->units().count();
    for (int i = 0; i < units; ++i) {
        m_unitSignalMapper->setMapping(m_course->units().at(i).get(), i);
    }
}

void PhraseModel::updatePhraseMappings()
{
    // TODO this might be quite costly for long units
    // better, implement access based on index pairs
    for (auto unit : m_course->units()) {
        for (const auto &phrase : unit->phrases()) {
            m_phraseSignalMapper->setMapping(phrase.get(), phrase.get());
        }
    }
}
