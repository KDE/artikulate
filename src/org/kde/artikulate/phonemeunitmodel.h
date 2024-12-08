/*
    SPDX-FileCopyrightText: 2013-2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PHONEMEUNITMODEL_H
#define PHONEMEUNITMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>

class PhonemeGroup;
class ICourse;
class Unit;
class PhonemeGroup;
class QSignalMapper;

class PhonemeUnitModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(PhonemeGroup *phonemeGroup READ phonemeGroup WRITE setPhonemeGroup NOTIFY phonemeGroupChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    QML_ELEMENT

public:
    enum unitRoles {
        TitleRole = Qt::UserRole + 1,
        NumberPhrasesRole,
        IdRole,
        DataRole,
        PhonemeGroupRole
    };

    explicit PhonemeUnitModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    void setCourse(ICourse *course);
    ICourse *course() const;
    void setPhonemeGroup(PhonemeGroup *phonemeGroup);
    PhonemeGroup *phonemeGroup() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int count() const;

Q_SIGNALS:
    void unitChanged(int index);
    void courseChanged();
    void phonemeGroupChanged();
    void countChanged();

private Q_SLOTS:
    void onUnitAboutToBeAdded(PhonemeGroup *phonemeGroup, int index);
    void onUnitAdded();
    void onUnitsAboutToBeRemoved(int first, int last);
    void onUnitsRemoved();
    void emitUnitChanged(int row);

private:
    void updateMappings();
    ICourse *m_course;
    PhonemeGroup *m_phonemeGroup;
    QSignalMapper *m_signalMapper;
};

#endif // PHONEMEUNITMODEL_H
