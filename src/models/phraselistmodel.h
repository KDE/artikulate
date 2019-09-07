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

#ifndef PHRASELISTMODEL_H
#define PHRASELISTMODEL_H

#include <QAbstractListModel>
#include "core/phrase.h"

class Unit;
class QSignalMapper;

class PhraseListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Unit *unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum phraseRoles {
        TextRole = Qt::UserRole + 1,
        IdRole,
        TypeRole,
        SoundFileRole,
        ExcludedRole,
        DataRole
    };

    explicit PhraseListModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int,QByteArray> roleNames() const override;
    void setUnit(Unit *unit);
    Unit * unit() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Count phrases in current model view. If this number is changed, signal
     * countChanged() is emitted.
     *
     * \return number of phrases
     */
    int count() const;

Q_SIGNALS:
    void phraseChanged(int index);
    void unitChanged();
    void typeChanged();
    void countChanged();

private Q_SLOTS:
    void onPhraseAboutToBeAdded(IPhrase *unit, int index);
    void onPhraseAdded();
    void onPhrasesAboutToBeRemoved(int first, int last);
    void onPhrasesRemoved();
    void emitPhraseChanged(int row);

private:
    void updateMappings();
    Unit *m_unit;
    QSignalMapper *m_signalMapper;
};

#endif
