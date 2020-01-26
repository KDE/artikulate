/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "phraselistmodel.h"
#include "core/phrase.h"
#include "core/unit.h"
#include <KLocalizedString>
#include <QAbstractListModel>
#include <QSignalMapper>

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

QHash<int, QByteArray> PhraseListModel::roleNames() const
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
        for (auto &phrase : m_unit->phrases()) {
            phrase->disconnect(this);
        }
    }

    m_unit = unit;
    if (m_unit) {
        // initial setting of signal mappings
        connect(m_unit, &Unit::phraseAboutToBeAdded, this, &PhraseListModel::onPhraseAboutToBeAdded);
        connect(m_unit, &Unit::phraseAdded, this, &PhraseListModel::onPhraseAdded);
        connect(m_unit, &Unit::phraseAboutToBeRemoved, this, &PhraseListModel::onPhraseAboutToBeRemoved);
        connect(m_unit, &Unit::phraseRemoved, this, &PhraseListModel::onPhrasesRemoved);

        // insert and connect all already existing phrases
        int phrases = m_unit->phrases().count();
        for (int i = 0; i < phrases; ++i) {
            onPhraseAboutToBeAdded(m_unit->phrases().at(i), i);
            endInsertRows();
            emit countChanged();
        }
        updateMappings();
    }

    // emit done
    endResetModel();
    emit unitChanged();
}

Unit *PhraseListModel::unit() const
{
    return m_unit;
}

QVariant PhraseListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_unit);

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_unit->phrases().count()) {
        return QVariant();
    }

    std::shared_ptr<IPhrase> const phrase = m_unit->phrases().at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return !phrase->text().isEmpty() ? QVariant(phrase->text()) : QVariant(i18nc("@item:inlistbox:", "unknown"));
        case Qt::ToolTipRole:
            return QVariant(phrase->text());
        case TextRole:
            return phrase->text();
        case SoundFileRole:
            return phrase->sound();
        case IdRole:
            return phrase->id();
        case TypeRole:
            return QVariant::fromValue<IPhrase::Type>(phrase->type());
            //    case ExcludedRole: //FIXME
            //        return phrase->isExcluded();
        case DataRole:
            return QVariant::fromValue<QObject *>(phrase.get());
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
    return m_unit->phrases().count();
}

void PhraseListModel::onPhraseAboutToBeAdded(std::shared_ptr<IPhrase> phrase, int index)
{
    connect(phrase.get(), SIGNAL(textChanged()), m_signalMapper, SLOT(map()));
    connect(phrase.get(), SIGNAL(typeChanged()), m_signalMapper, SLOT(map()));
    connect(phrase.get(), SIGNAL(excludedChanged()), m_signalMapper, SLOT(map()));
    beginInsertRows(QModelIndex(), index, index);
}

void PhraseListModel::onPhraseAdded()
{
    updateMappings();
    endInsertRows();
    emit countChanged();
}

void PhraseListModel::onPhraseAboutToBeRemoved(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
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
    // FIXME very inefficient, but workaround to force new filtering in phrasefiltermodel
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
    return m_unit->phrases().count();
}

void PhraseListModel::updateMappings()
{
    if (!m_unit) {
        return;
    }
    int phrases = m_unit->phrases().count();
    for (int i = 0; i < phrases; ++i) {
        m_signalMapper->setMapping(m_unit->phrases().at(i).get(), i);
    }
}
