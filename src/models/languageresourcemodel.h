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

#ifndef LANGUAGERESOURCEMODEL_H
#define LANGUAGERESOURCEMODEL_H

#include <QAbstractListModel>

class ResourceManager;
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
    void setResourceManager(ResourceManager *resourceManager);
    ResourceManager * resourceManager() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
    void languageChanged(int index);
    void resourceManagerChanged();

private slots:
    void onLanguageAboutToBeAdded(Language *language, int index);
    void onLanguageAdded();
    void onLanguagesAboutToBeRemoved(int first, int last);
    void onLanguagesRemoved();
    void emitLanguageChanged(int row);

private:
    void updateMappings();
    ResourceManager *m_resourceManager;
    QSignalMapper *m_signalMapper;
};

#endif
