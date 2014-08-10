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

#ifndef LEARNINGPROGRESSMODEL_H
#define LEARNINGPROGRESSMODEL_H

#include <QAbstractTableModel>
#include <QAbstractItemModel>

class TrainingSession;

class LearningProgressModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(TrainingSession *session READ session WRITE setSession NOTIFY sessionChanged)
    Q_PROPERTY(int maximumTries READ maximumTries NOTIFY maximumTriesChanged)
    Q_PROPERTY(int maximumPhrasesPerTry READ maximumPhrasesPerTry NOTIFY maximumTriesChanged)

public:
    enum unitRoles {
        TitleRole = Qt::UserRole + 1,
        NumberPhrasesRole,
        IdRole,
        DataRole
    };

    explicit LearningProgressModel(QObject *parent = 0);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int,QByteArray> roleNames() const;
    void setSession(TrainingSession *session);
    TrainingSession * session() const;
    int maximumTries() const;
    int maximumPhrasesPerTry() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

Q_SIGNALS:
    void sessionChanged();
    void maximumTriesChanged();

private Q_SLOTS:
    void updateResults();

private:
    TrainingSession *m_session;
    int m_maximumTries;
};

#endif // LEARNINGPROGRESSMODEL_H
