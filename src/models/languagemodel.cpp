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



#include "languagemodel.h"
#include "core/language.h"
#include "core/course.h"
#include <core/resourcemanager.h>

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

LanguageModel::LanguageModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_resourceManager(0)
    , m_signalMapper(new QSignalMapper(this))
{
    kDebug() << "create language model";

    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    setRoleNames(roles);

    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitLanguageChanged(int)));
}

void LanguageModel::setResourceManager(ResourceManager *resourceManager)
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
        //FIXME
        connect(m_resourceManager, SIGNAL(languageAboutToBeAdded(Language*,int)), SLOT(onLanguageAboutToBeAdded(Language*,int)));
        connect(m_resourceManager, SIGNAL(languageAdded()), SLOT(onLanguageAdded()));
        connect(m_resourceManager, SIGNAL(languageAboutToBeRemoved(int,int)), SLOT(onLanguagesAboutToBeRemoved(int,int)));
        connect(m_resourceManager, SIGNAL(languageRemoved()), SLOT(onLanguagesRemoved()));
    }

    endResetModel();

    emit resourceManagerChanged();
}

ResourceManager * LanguageModel::resourceManager() const
{
    return m_resourceManager;
}

QVariant LanguageModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_resourceManager->languageList().count()) {
        return QVariant();
    }

    Language * const language = m_resourceManager->language(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !language->title().isEmpty()?
                QVariant(language->title()): QVariant(i18n("<No title>"));
    case Qt::ToolTipRole:
        return QVariant(i18n("<p>%1</p>", language->title()));
    case TitleRole:
        return language->title();
    case IdRole:
        return language->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(language);
    default:
        return QVariant();
    }
}

int LanguageModel::rowCount(const QModelIndex& parent) const
{
    if (!m_resourceManager) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }

    return m_resourceManager->languageList().count();
}

void LanguageModel::onLanguageAboutToBeAdded(Language *language, int index)
{
    connect(language, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    connect(language, SIGNAL(tagsChanged()), m_signalMapper, SLOT(map()));
    connect(language, SIGNAL(groupsChanged()), m_signalMapper, SLOT(map()));
    beginInsertRows(QModelIndex(), index, index);
}

void LanguageModel::onLanguageAdded()
{
    updateMappings();
    endInsertRows();
}

void LanguageModel::onLanguagesAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void LanguageModel::onLanguagesRemoved()
{
    endRemoveRows();
}

void LanguageModel::emitLanguageChanged(int row)
{
    emit languageChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant LanguageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18n("Title"));
}

void LanguageModel::updateMappings()
{
    int languages = m_resourceManager->languageList().count();
    for (int i = 0; i < languages; i++)
    {
        m_signalMapper->setMapping(m_resourceManager->language(i), i);
    }
}

