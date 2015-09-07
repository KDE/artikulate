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

#ifndef PHONEMEGROUPMODEL_H
#define PHONEMEGROUPMODEL_H

#include <QAbstractListModel>

class Course;
class PhonemeGroup;
class QSignalMapper;

class PhonemeGroupModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Course *course READ course WRITE setCourse NOTIFY courseChanged)

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
    virtual QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    void setCourse(Course *course);
    Course * course() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

signals:
    void phonemeGroupChanged(int index);
    void courseChanged();

private slots:
    void onPhonemeGroupAboutToBeAdded(PhonemeGroup *phonemeGroup, int index);
    void onPhonemeGroupAdded();
    void onPhonemeGroupsAboutToBeRemoved(int first, int last);
    void onPhonemeGroupsRemoved();
    void emitPhonemeGroupChanged(int row);

private:
    void updateMappings();
    Course *m_course;
    QSignalMapper *m_signalMapper;
};

#endif // PHONEMEGROUPMODEL_H
