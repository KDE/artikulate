/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef LEARNINGPROGRESSMODEL_H
#define LEARNINGPROGRESSMODEL_H

#include <QAbstractTableModel>

class TrainingSession;

class LearningProgressModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(TrainingSession *session READ session WRITE setSession NOTIFY sessionChanged)
    Q_PROPERTY(int maximumTries READ maximumTries NOTIFY maximumTriesChanged)
    Q_PROPERTY(int maximumPhrasesPerTry READ maximumPhrasesPerTry NOTIFY maximumTriesChanged)

public:
    enum unitRoles { TitleRole = Qt::UserRole + 1, NumberPhrasesRole, IdRole, DataRole };

    explicit LearningProgressModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    void setSession(TrainingSession *session);
    TrainingSession *session() const;
    int maximumTries() const;
    int maximumPhrasesPerTry() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

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
