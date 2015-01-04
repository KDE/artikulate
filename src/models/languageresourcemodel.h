/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef LANGUAGERESOURCEMODEL_H
#define LANGUAGERESOURCEMODEL_H

#include <QAbstractListModel>
#include "languagemodel.h"

class ResourceManager;
class LanguageResource;
class Language;
class QSignalMapper;

class LanguageResourceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ResourceManager *resourceManager READ resourceManager WRITE setResourceManager NOTIFY resourceManagerChanged)

public:
    enum LanguageRoles {
        TitleRole = Qt::UserRole + 1,
        I18nTitleRole,
        IdRole,
        DataRole,
        CourseNumberRole
    };

    explicit LanguageResourceModel(QObject *parent = 0);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    void setResourceManager(ResourceManager *resourceManager);
    ResourceManager * resourceManager() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    void setView(LanguageModel::LanguageResourceView view);
    LanguageModel::LanguageResourceView view() const;

Q_SIGNALS:
    void languageChanged(int index);
    void resourceManagerChanged();

private Q_SLOTS:
    void onLanguageResourceAboutToBeAdded(LanguageResource *resource, int index);
    void onLanguageResourceAdded();
    void onLanguageResourceAboutToBeRemoved(int index);
    void onLanguageResourceRemoved();
    void emitLanguageChanged(int row);
    void updateDisplayedLanguages();

private:
    bool displayResource(LanguageResource *resource) const;
    void updateResources();
    void updateMappings();
    ResourceManager *m_resourceManager;
    QList<LanguageResource *> m_resources;
    LanguageModel::LanguageResourceView m_view;
    QSignalMapper *m_signalMapper;
};

#endif
