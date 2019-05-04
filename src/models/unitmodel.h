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

#ifndef UNITMODEL_H
#define UNITMODEL_H

#include <QAbstractListModel>

class ICourse;
class Unit;
class QSignalMapper;

class UnitModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)

public:
    enum unitRoles {
        TitleRole = Qt::UserRole + 1,   //!< title of unit
        IdRole,                         //!< unique identifier of unit
        ContainsTrainingData,           //!< boolean value indicating whether unit has phrase with native recordings
        DataRole                        //!< access to Unit object
    };

    explicit UnitModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int,QByteArray> roleNames() const override;
    void setCourse(ICourse *course);
    ICourse * course() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void unitChanged(int index);
    void courseChanged();

private Q_SLOTS:
    void onUnitAboutToBeAdded(Unit *unit, int index);
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
