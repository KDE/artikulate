/*
 *  SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef UNITMODEL_H
#define UNITMODEL_H

#include <QAbstractListModel>
#include <memory>

class ICourse;
class Unit;
class QSignalMapper;

class UnitModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)

public:
    enum unitRoles {
        TitleRole = Qt::UserRole + 1, //!< title of unit
        IdRole,                       //!< unique identifier of unit
        ContainsTrainingData,         //!< boolean value indicating whether unit has phrase with native recordings
        DataRole                      //!< access to Unit object
    };

    explicit UnitModel(QObject *parent = nullptr);
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
    void unitChanged(int index);
    void courseChanged();

private Q_SLOTS:
    void onUnitAboutToBeAdded(std::shared_ptr<Unit> unit, int index);
    void onUnitAdded();
    void onUnitsAboutToBeRemoved(int first, int last);
    void onUnitsRemoved();
    void emitUnitChanged(int row);

private:
    void updateMappings();
    ICourse *m_course;
    QSignalMapper *m_signalMapper;
};

#endif // UNITMODEL_H
