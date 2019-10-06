/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "languageresourcemodel.h"
#include "application.h"
#include "core/iresourcerepository.h"
#include "core/language.h"

#include "artikulate_debug.h"
#include <KLocalizedString>
#include <QAbstractListModel>
#include <QSignalMapper>

LanguageResourceModel::LanguageResourceModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_repository(nullptr)
    , m_view(LanguageModel::NonEmptyGhnsOnlyLanguages)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitLanguageChanged(int)));
    setResourceRepository(artikulateApp->resourceRepository());
}

QHash<int, QByteArray> LanguageResourceModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[I18nTitleRole] = "i18nTitle";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    roles[CourseNumberRole] = "courseNumberRole";

    return roles;
}

void LanguageResourceModel::setResourceRepository(IResourceRepository *repository)
{
    if (m_repository == repository) {
        return;
    }

    if (m_repository) {
        m_repository->disconnect(this);
    }
    m_repository = repository;
    updateDisplayedLanguages();

    emit resourceRepositoryChanged();
}

IResourceRepository *LanguageResourceModel::resourceRepository() const
{
    return m_repository;
}

QVariant LanguageResourceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_languages.count()) {
        return QVariant();
    }

    ILanguage *const language = m_languages.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return !language->title().isEmpty() ? QVariant(language->title()) : QVariant(i18nc("@item:inlistbox:", "unknown"));
        case Qt::ToolTipRole:
            return QVariant(language->title());
        case TitleRole:
            return language->title();
        case I18nTitleRole:
            return language->i18nTitle();
        case IdRole:
            return language->id();
        case DataRole:
            return QVariant::fromValue<QObject *>(language);
        case CourseNumberRole:
            return m_languages.count();
        default:
            return QVariant();
    }
}

int LanguageResourceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_languages.count();
}

void LanguageResourceModel::onLanguageAboutToBeAdded(Language *language, int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_languages.append(language);

    connect(language, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    connect(language, SIGNAL(phonemesChanged()), m_signalMapper, SLOT(map()));
    connect(language, SIGNAL(phonemeGroupsChanged()), m_signalMapper, SLOT(map()));
}

void LanguageResourceModel::onLanguageAdded()
{
    updateMappings();
    endInsertRows();
}

void LanguageResourceModel::onLanguageAboutToBeRemoved(int index)
{
    if (!m_repository) {
        return;
    }

    ILanguage *originalLanguage = m_repository->languages().at(index).get();
    int modelIndex = m_languages.indexOf(originalLanguage);

    if (modelIndex == -1) {
        qCWarning(ARTIKULATE_LOG()) << "Cannot remove language from model, not registered";
        return;
    }
    beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
    originalLanguage->disconnect(m_signalMapper);
    m_languages.removeAt(modelIndex);
}

void LanguageResourceModel::onLanguageRemoved()
{
    endRemoveRows();
}

void LanguageResourceModel::emitLanguageChanged(int row)
{
    emit languageChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant LanguageResourceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Language"));
}

void LanguageResourceModel::setView(LanguageModel::LanguageResourceView view)
{
    if (m_view == view) {
        return;
    }
    m_view = view;
    updateDisplayedLanguages();
}

void LanguageResourceModel::updateDisplayedLanguages()
{
    beginResetModel();
    m_languages.clear();
    if (m_repository) {
        m_languages.clear();
        for (auto language : m_repository->languages()) {
            m_languages.append(language.get());
        }
    }
    updateMappings();
    endResetModel();
}

LanguageModel::LanguageResourceView LanguageResourceModel::view() const
{
    return m_view;
}

void LanguageResourceModel::updateMappings()
{
    int languages = m_languages.count();
    for (int i = 0; i < languages; i++) {
        m_signalMapper->setMapping(m_languages.at(i), i);
    }
}
