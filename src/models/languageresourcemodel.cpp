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
#include "core/language.h"
#include "core/course.h"
#include "core/resourcemanager.h"
#include "core/resources/languageresource.h"
#include "core/resources/courseresource.h"

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <QDebug>

LanguageResourceModel::LanguageResourceModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_resourceManager(0)
    , m_view(LanguageModel::NonEmptyGhnsOnlyLanguages)
    , m_signalMapper(new QSignalMapper(this))
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[I18nTitleRole] = "i18nTitle";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    roles[CourseNumberRole] = "courseNumberRole";
    setRoleNames(roles);

    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitLanguageChanged(int)));
}

void LanguageResourceModel::setResourceManager(ResourceManager *resourceManager)
{
    if (m_resourceManager == resourceManager) {
        return;
    }

    beginResetModel();

    if (m_resourceManager) {
        m_resourceManager->disconnect(this);
    }

    m_resourceManager = resourceManager;

    if (m_resourceManager) {
        connect(m_resourceManager, SIGNAL(languageResourceAboutToBeAdded(LanguageResource*,int)),
                SLOT(onLanguageResourceAboutToBeAdded(LanguageResource*,int)));
        connect(m_resourceManager, SIGNAL(languageResourceAdded()),
                SLOT(onLanguageResourceAdded()));
        connect(m_resourceManager, SIGNAL(languageResourceAboutToBeRemoved(int)),
                SLOT(onLanguageResourceAboutToBeRemoved(int)));
        connect(m_resourceManager, SIGNAL(languageResourceRemoved()),
                SLOT(onLanguageResourceRemoved()));
        connect(m_resourceManager, SIGNAL(languageCoursesChanged()),
                SLOT(updateDisplayedLanguages()));
    }
    updateResources();

    endResetModel();

    emit resourceManagerChanged();
}

ResourceManager * LanguageResourceModel::resourceManager() const
{
    return m_resourceManager;
}

QVariant LanguageResourceModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_resources.count()) {
        return QVariant();
    }

    Language * const language = m_resources.at(index.row())->language();
    switch(role)
    {
    case Qt::DisplayRole:
        return !language->title().isEmpty() ?
                QVariant(language->title()): QVariant(i18nc("@item:inlistbox:", "unknown"));
    case Qt::ToolTipRole:
        return QVariant(language->title());
    case TitleRole:
        return language->title();
    case I18nTitleRole:
        return language->i18nTitle();
    case IdRole:
        return language->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(language);
    case CourseNumberRole:
        return m_resources.count();
    default:
        return QVariant();
    }
}

int LanguageResourceModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_resources.count();
}

void LanguageResourceModel::onLanguageResourceAboutToBeAdded(LanguageResource *resource, int index)
{
    if (!displayResource(resource)) {
        return;
    }

    beginInsertRows(QModelIndex(), index, index);
    m_resources.append(resource);

    connect(resource->language(), SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    connect(resource->language(), SIGNAL(phonemesChanged()), m_signalMapper, SLOT(map()));
    connect(resource->language(), SIGNAL(phonemeGroupsChanged()), m_signalMapper, SLOT(map()));
}

void LanguageResourceModel::onLanguageResourceAdded()
{
    updateMappings();
    endInsertRows();
}

void LanguageResourceModel::onLanguageResourceAboutToBeRemoved(int index)
{
    if (!m_resourceManager) {
        return;
    }

    LanguageResource *originalResource = m_resourceManager->languageResources().at(index);
    int modelIndex = m_resources.indexOf(originalResource);

    if (modelIndex == -1) {
        qWarning() << "Cannot remove language from model, not registered";
        return;
    }
    beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
    originalResource->disconnect(m_signalMapper);
    m_resources.removeAt(modelIndex);
}

void LanguageResourceModel::onLanguageResourceRemoved()
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
    emit beginResetModel();
    m_view = view;
    updateResources();
    emit endResetModel();
}

void LanguageResourceModel::updateDisplayedLanguages()
{
    emit beginResetModel();
    updateResources();
    emit endResetModel();
}

LanguageModel::LanguageResourceView LanguageResourceModel::view() const
{
    return m_view;
}

bool LanguageResourceModel::displayResource(LanguageResource* resource) const
{
    if (m_view == LanguageModel::AllLanguages) {
        return true;
    }

    // otherwise compute data needed for decision
    QList<CourseResource*> courses = m_resourceManager->courseResources(resource->language());
    int contribCount = 0;

    if (m_view == LanguageModel::NonEmptyLanguages
        && courses.count() > 0)
    {
        return true;
    }

    // compute data for determining whether language shall be shown or not
    foreach (CourseResource *course, courses) {
        if (course->isContributorResource()) {
            ++contribCount;
        }
    }

    if (m_view == LanguageModel::NonEmptyContributorOnlyResources
        && contribCount > 0)
    {
        return true;
    }

    if (m_view == LanguageModel::NonEmptyGhnsOnlyLanguages
        && courses.count() - contribCount > 0)
    {
        return true;
    }

    return false;
}

void LanguageResourceModel::updateResources()
{
    if (!m_resourceManager) {
        return;
    }

    m_resources.clear();
    QList<LanguageResource*> resources = m_resourceManager->languageResources();

    foreach (LanguageResource *language, resources) {
        if (displayResource(language)) {
            m_resources.append(language);
        }
    }
    updateMappings();
}

void LanguageResourceModel::updateMappings()
{
    int languages = m_resources.count();
    for (int i = 0; i < languages; i++) {
        m_signalMapper->setMapping(m_resources.at(i), i);
    }
}
