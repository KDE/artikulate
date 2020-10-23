/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "phonememodel.h"
#include "core/language.h"
#include "core/phoneme.h"

#include <QSignalMapper>

#include "artikulate_debug.h"
#include <KLocalizedString>

PhonemeModel::PhonemeModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_language(nullptr)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitPhonemeChanged(int)));
}

QHash<int, QByteArray> PhonemeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";

    return roles;
}

Language *PhonemeModel::language() const
{
    return m_language;
}

void PhonemeModel::setLanguage(Language *language)
{
    beginResetModel();
    m_language = language;
    emit languageChanged();
    endResetModel();
}

QVariant PhonemeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_language->phonemes().count()) {
        return QVariant();
    }

    auto phoneme = m_language->phonemes().at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return !phoneme->title().isEmpty() ? QVariant(phoneme->title()) : QVariant(i18nc("@item:inlistbox:", "unknown"));
        case Qt::ToolTipRole:
            return QVariant(phoneme->title());
        case TitleRole:
            return phoneme->title();
        case IdRole:
            return phoneme->id();
        case DataRole:
            return QVariant::fromValue<QObject *>(phoneme.get());
        default:
            return QVariant();
    }
}

int PhonemeModel::rowCount(const QModelIndex &parent) const
{
    if (!m_language) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }
    return m_language->phonemes().count();
}

void PhonemeModel::onPhonemeAboutToBeAdded(Phoneme *phoneme, int index)
{
    connect(phoneme, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    beginInsertRows(QModelIndex(), index, index);
}

void PhonemeModel::onPhonemeAdded()
{
    updateMappings();
    endInsertRows();
}

void PhonemeModel::onPhonemesAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void PhonemeModel::onPhonemesRemoved()
{
    endRemoveRows();
}

void PhonemeModel::emitPhonemeChanged(int row)
{
    emit phonemeChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant PhonemeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Phoneme"));
}

void PhonemeModel::updateMappings()
{
    if (!m_language) {
        qCDebug(ARTIKULATE_LOG) << "Aborting to update mappings, language not set.";
        return;
    }
    int phonemes = m_language->phonemes().count();
    for (int i = 0; i < phonemes; i++) {
        m_signalMapper->setMapping(m_language->phonemes().at(i).get(), i);
    }
}
