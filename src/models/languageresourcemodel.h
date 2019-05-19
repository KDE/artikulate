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

class IResourceRepository;
class LanguageResource;
class Language;
class QSignalMapper;

class LanguageResourceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(IResourceRepository *repository READ resourceRepository WRITE setResourceRepository NOTIFY resourceRepositoryChanged)

public:
    enum LanguageRoles {
        TitleRole = Qt::UserRole + 1,
        I18nTitleRole,
        IdRole,
        DataRole,
        CourseNumberRole
    };

    explicit LanguageResourceModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int,QByteArray> roleNames() const override;
    void setResourceRepository(IResourceRepository *repository);
    IResourceRepository * resourceRepository() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void setView(LanguageModel::LanguageResourceView view);
    LanguageModel::LanguageResourceView view() const;

Q_SIGNALS:
    void languageChanged(int index);
    void resourceRepositoryChanged();

private Q_SLOTS:
    void onLanguageResourceAboutToBeAdded(LanguageResource *resource, int index);
    void onLanguageResourceAdded();
    void onLanguageResourceAboutToBeRemoved(int index);
    void onLanguageResourceRemoved();
    void emitLanguageChanged(int row);
    void updateDisplayedLanguages();

private:
    void updateResources();
    void updateMappings();
    IResourceRepository *m_repository;
    QList<LanguageResource *> m_resources;
    LanguageModel::LanguageResourceView m_view;
    QSignalMapper *m_signalMapper;
};

#endif
