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

#ifndef PHRASEMODEL_H
#define PHRASEMODEL_H

#include <QAbstractItemModel>
#include "core/phrase.h"

class Course;
class QSignalMapper;

class PhraseModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(Course *course READ course WRITE setCourse NOTIFY courseChanged)

public:
    enum phraseRoles {
        TextRole = Qt::UserRole + 1,
        IdRole,
        DataRole
    };

    explicit PhraseModel(QObject *parent = nullptr);
    virtual QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    void setCourse(Course *course);
    Course * course() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Q_INVOKABLE bool isPhrase(const QModelIndex &index) const;
    Q_INVOKABLE Phrase * phrase(const QModelIndex &index) const;
    Q_INVOKABLE Unit * unit(const QModelIndex &index) const;
    Q_INVOKABLE QModelIndex indexPhrase(Phrase *phrase) const;
    Q_INVOKABLE QModelIndex indexUnit(Unit *unit) const;
    Q_INVOKABLE bool isUnit(const QModelIndex& index) const;

Q_SIGNALS:
    void phraseChanged(int index);
    void courseChanged();
    void typeChanged();

private Q_SLOTS:
    void onPhraseAboutToBeAdded(Phrase *phrase, int index);
    void onPhraseAdded();
    void onPhrasesAboutToBeRemoved(int first, int last);
    void onPhrasesRemoved();
    void onPhraseChanged(QObject *phrase);
    void onUnitAboutToBeAdded(Unit *unit, int index);
    void onUnitAdded();
    void onUnitsAboutToBeRemoved(int first, int last);
    void onUnitsRemoved();
    void onUnitChanged(int index);

private:
    void updateUnitMappings();
    void updatePhraseMappings();

    Course *m_course;
    QSignalMapper *m_unitSignalMapper;
    QSignalMapper *m_phraseSignalMapper;
};

#endif
