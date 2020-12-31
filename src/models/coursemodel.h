/*
    SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef COURSEMODEL_H
#define COURSEMODEL_H

#include "artikulatecore_export.h"
#include <QAbstractListModel>
#include <memory>

class IResourceRepository;
class ICourse;
class Language;

class ARTIKULATECORE_EXPORT CourseModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum courseRoles { TitleRole = Qt::UserRole + 1, I18nTitleRole, DescriptionRole, IdRole, LanguageRole, DataRole };

    explicit CourseModel(QObject *parent = nullptr);
    explicit CourseModel(IResourceRepository *repository, QObject *parent = nullptr);
    ~CourseModel() override = default;
    QHash<int, QByteArray> roleNames() const override;
    IResourceRepository *resourceRepository() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant course(int index) const;

protected:
    void setResourceRepository(IResourceRepository *resourceRepository);

private Q_SLOTS:
    void onCourseAboutToBeAdded(std::shared_ptr<ICourse> course, int index);
    void onCourseAdded();
    void onCourseAboutToBeRemoved(int row);

private:
    IResourceRepository *m_resourceRepository {nullptr};
    QVector<QMetaObject::Connection> m_updateConnections;
};

#endif // COURSEMODEL_H
