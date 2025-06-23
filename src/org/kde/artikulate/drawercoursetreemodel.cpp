// SPDX-FileCopyrightText: 2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "drawercoursetreemodel.h"
#include "core/icourse.h"
#include "core/phrase.h"
#include "core/unit.h"
#include "editorsession.h"
#include <KLocalizedString>
#include <QList>

void DrawerCourseTreeModel::setCourse(ICourse *course)
{
    if (course == m_course) {
        return;
    }
    m_course = course;
    beginResetModel();
    rebuildModel();
    endResetModel();
    Q_EMIT courseChanged();
}

ICourse *DrawerCourseTreeModel::course() const
{
    return m_course;
}

void DrawerCourseTreeModel::setSession(ISessionActions *session)
{
    if (session == m_session) {
        return;
    }
    if (m_session) {
        disconnect(session, &ISessionActions::phraseChanged, this, &DrawerCourseTreeModel::handleSessionPhraseChanged);
    }
    m_session = session;
    beginResetModel();
    rebuildModel();
    endResetModel();
    Q_EMIT courseChanged();
    connect(session, &ISessionActions::phraseChanged, this, &DrawerCourseTreeModel::handleSessionPhraseChanged);
}

ISessionActions *DrawerCourseTreeModel::session() const
{
    return m_session;
}

void DrawerCourseTreeModel::setTrainingFilter(bool trainingFilter)
{
    if (m_trainingFilter == trainingFilter) {
        return;
    }
    m_trainingFilter = trainingFilter;
    rebuildModel();
    Q_EMIT trainingFilterChanged();
}

void DrawerCourseTreeModel::handleSessionPhraseChanged()
{
    for (int unitIndex = 0; unitIndex < m_rootItem->childCount(); ++unitIndex) {
        std::shared_ptr<SelectionEntry> unit = m_rootItem->child(unitIndex);
        for (int phraseIndex = 0; phraseIndex < unit->childCount(); ++phraseIndex) {
            std::shared_ptr<SelectionEntry> phrase = unit->child(phraseIndex);
            if (unit->child(phraseIndex)->phrase().get() == m_session->activePhrase()) {
                QModelIndex unitModelIndex = index(unitIndex, 0, QModelIndex());
                auto needle = index(phraseIndex, 0, unitModelIndex);
                Q_EMIT currentIndexChanged(needle);
                return;
            }
        }
    }
}

bool DrawerCourseTreeModel::trainingFilter() const
{
    return m_trainingFilter;
}

SelectionEntry::SelectionEntry(const QString &text, std::shared_ptr<IUnit> data, std::shared_ptr<SelectionEntry> parent)
    : m_parentItem(parent)
    , m_text(text)
    , m_unit(data)
{
}

SelectionEntry::SelectionEntry(const QString &text, std::shared_ptr<IPhrase> data, std::shared_ptr<SelectionEntry> parent)
    : m_parentItem(parent)
    , m_text(text)
    , m_phrase(data)
{
}

void SelectionEntry::appendChild(std::shared_ptr<SelectionEntry> item)
{
    m_childItems.push_back(item);
}

std::shared_ptr<SelectionEntry> SelectionEntry::child(int row)
{
    if (row < 0 || static_cast<std::size_t>(row) >= m_childItems.size()) {
        return nullptr;
    }
    return m_childItems.at(row);
}

int SelectionEntry::childCount() const
{
    return m_childItems.size();
}

int SelectionEntry::row() const
{
    auto parent = m_parentItem.lock();
    if (parent) {
        for (std::size_t i = 0; i < parent->m_childItems.size(); ++i) {
            if (parent->m_childItems.at(i).get() == this) {
                return i;
            }
        }
    }
    return 0;
}

int SelectionEntry::columnCount() const
{
    return 1;
}

QVariant SelectionEntry::data(Qt::ItemDataRole itemRole) const
{
    const auto role = static_cast<DrawerCourseTreeModel::Roles>(itemRole);
    switch (role) {
    case DrawerCourseTreeModel::Roles::Text:
        return QVariant::fromValue(m_text);
    case DrawerCourseTreeModel::Roles::ToolTip:
        return QVariant::fromValue(m_toolTip);
    }
    return QVariant();
}

std::shared_ptr<SelectionEntry> SelectionEntry::parentItem()
{
    return m_parentItem.lock();
}

std::shared_ptr<IPhrase> SelectionEntry::phrase()
{
    if (m_phrase.has_value()) {
        return m_phrase.value();
    }
    return std::shared_ptr<IPhrase>();
}

std::shared_ptr<IUnit> SelectionEntry::unit()
{
    if (m_unit.has_value()) {
        return m_unit.value();
    }
    return std::shared_ptr<IUnit>();
}

void DrawerCourseTreeModel::rebuildModel()
{
    m_rootItem = std::unique_ptr<SelectionEntry>(new SelectionEntry);
    if (!m_course) {
        return;
    }

    const std::function<bool(std::shared_ptr<IPhrase>)> is_training_phrase = [](std::shared_ptr<IPhrase> phrase) {
        return !phrase->sound().isEmpty();
    };

    for (const auto &unit : m_course->units()) {
        const auto phrases = unit->phrases();
        auto it = std::find_if(std::cbegin(phrases), std::cend(phrases), is_training_phrase);
        if (m_trainingFilter && it == phrases.cend()) {
            continue;
        }

        auto unitEntry = std::make_shared<SelectionEntry>(unit->title(), unit, m_rootItem);
        m_rootItem->appendChild(unitEntry);
        for (const auto &phrase : phrases) {
            if (m_trainingFilter && !is_training_phrase(phrase)) {
                continue;
            }
            auto phraseEntry = std::make_shared<SelectionEntry>(phrase->text(), phrase, unitEntry);
            unitEntry->appendChild(phraseEntry);
        }
    }
}

DrawerCourseTreeModel::DrawerCourseTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    beginResetModel();
    rebuildModel();
    endResetModel();
}

DrawerCourseTreeModel::~DrawerCourseTreeModel() = default;

QHash<int, QByteArray> DrawerCourseTreeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::ItemDataRole::DisplayRole] = "text";
    roles[Qt::ItemDataRole::ToolTipRole] = "tooltip";
    return roles;
}

QModelIndex DrawerCourseTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    SelectionEntry *parentItem;

    if (!parent.isValid()) {
        parentItem = m_rootItem.get();
    } else {
        parentItem = static_cast<SelectionEntry *>(parent.internalPointer());
    }

    std::shared_ptr<SelectionEntry> childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem.get());
    }
    return QModelIndex();
}

QModelIndex DrawerCourseTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    SelectionEntry *childItem = static_cast<SelectionEntry *>(index.internalPointer());
    std::shared_ptr<SelectionEntry> parentItem = childItem->parentItem();

    if (parentItem.get() == m_rootItem.get()) {
        return QModelIndex();
    }
    return createIndex(parentItem->row(), 0, parentItem.get());
}

int DrawerCourseTreeModel::rowCount(const QModelIndex &parent) const
{
    int rowCount{0};
    if (!parent.isValid()) {
        rowCount = m_rootItem->childCount();
    } else {
        auto parentItem = static_cast<SelectionEntry *>(parent.internalPointer());
        rowCount = parentItem->childCount();
    }
    return rowCount;
}

int DrawerCourseTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant DrawerCourseTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.parent().isValid()) {
        if (index.row() < 0 || index.row() >= m_rootItem->childCount()) {
            qCritical() << "Index out of range" << index;
            return QVariant();
        }
        return m_rootItem->child(index.row())->data(static_cast<Qt::ItemDataRole>(role));
    } else {
        auto entry = static_cast<SelectionEntry *>(index.internalPointer());
        if (nullptr != entry) {
            return entry->data(static_cast<Qt::ItemDataRole>(role));
        }
    }
    return QVariant();
}

QModelIndex DrawerCourseTreeModel::next(const QModelIndex &currentIndex) const
{
    QModelIndex nextPhrase;
    if (!currentIndex.isValid() && rowCount() > 0) { // initial phrase
        nextPhrase = firstPhraseIndexInUnit(index(0, 0));
    } else if (currentIndex.isValid() && !currentIndex.parent().isValid()) {
        nextPhrase = firstPhraseIndexInUnit(currentIndex);
    } else { // computing next for phrase
        if (currentIndex.row() < rowCount(currentIndex.parent()) - 1) {
            nextPhrase = index(currentIndex.row() + 1, 0, currentIndex.parent());
        } else if (currentIndex.parent().row() < rowCount() - 1) {
            nextPhrase = firstPhraseIndexInUnit(index(currentIndex.parent().row() + 1, 0));
        }
    }
    return nextPhrase;
}

void DrawerCourseTreeModel::trigger(const QModelIndex &index)
{
    if (m_session) {
        auto entry = static_cast<SelectionEntry *>(index.internalPointer());
        const auto phrase = entry->phrase();
        if (phrase) {
            m_session->setActivePhrase(phrase.get());
        } else {
            const auto unit = entry->unit();
            auto editorSession = qobject_cast<EditorSession *>(m_session);
            if (unit && editorSession) {
                editorSession->setActiveUnit(unit.get());
            }
        }
    }
}

QModelIndex DrawerCourseTreeModel::firstPhraseIndexInUnit(const QModelIndex &currentIndex) const
{
    Q_ASSERT(currentIndex.isValid());
    Q_ASSERT(!currentIndex.parent().isValid());
    if (rowCount(currentIndex) > 0) {
        return index(0, 0, currentIndex);
    } else {
        return QModelIndex();
    }
}
