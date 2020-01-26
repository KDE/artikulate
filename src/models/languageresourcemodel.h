/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef LANGUAGERESOURCEMODEL_H
#define LANGUAGERESOURCEMODEL_H

#include "languagemodel.h"
#include <QAbstractListModel>

class IResourceRepository;
class ILanguage;
class QSignalMapper;

class LanguageResourceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(IResourceRepository *repository READ resourceRepository WRITE setResourceRepository NOTIFY resourceRepositoryChanged)

public:
    enum LanguageRoles { TitleRole = Qt::UserRole + 1, I18nTitleRole, IdRole, DataRole, CourseNumberRole };

    explicit LanguageResourceModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    void setResourceRepository(IResourceRepository *repository);
    IResourceRepository *resourceRepository() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void setView(LanguageModel::LanguageResourceView view);
    LanguageModel::LanguageResourceView view() const;

Q_SIGNALS:
    void languageChanged(int index);
    void resourceRepositoryChanged();

private Q_SLOTS:
    void onLanguageAboutToBeAdded(Language *resource, int index);
    void onLanguageAdded();
    void onLanguageAboutToBeRemoved(int index);
    void onLanguageRemoved();
    void emitLanguageChanged(int row);
    void updateDisplayedLanguages();

private:
    void updateMappings();
    IResourceRepository *m_repository;
    QVector<ILanguage *> m_languages;
    LanguageModel::LanguageResourceView m_view;
    QSignalMapper *m_signalMapper;
};

#endif
