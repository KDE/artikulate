/*
    SPDX-FileCopyrightText: 2013-2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PHONEMEGROUPMODEL_H
#define PHONEMEGROUPMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>

class ICourse;
class PhonemeGroup;
class QSignalMapper;

class PhonemeGroupModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)

    QML_ELEMENT

public:
    enum unitRoles {
        TitleRole = Qt::UserRole + 1,
        IdRole,
        DataRole
    };

    explicit PhonemeGroupModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    void setCourse(ICourse *course);
    ICourse *course() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void phonemeGroupChanged(int index);
    void courseChanged();

private Q_SLOTS:
    void onPhonemeGroupAboutToBeAdded(PhonemeGroup *phonemeGroup, int index);
    void onPhonemeGroupAdded();
    void onPhonemeGroupsAboutToBeRemoved(int first, int last);
    void onPhonemeGroupsRemoved();
    void onPhonemeGroupChanged(int row);

private:
    void updateMappings();
    ICourse *m_course;
    QSignalMapper *m_signalMapper;
};

#endif // PHONEMEGROUPMODEL_H
