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

#ifndef PHRASEMODEL_H
#define PHRASEMODEL_H

#include <QAbstractListModel>
#include "core/phrase.h"

class Unit;
class QSignalMapper;

class PhraseModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Unit *unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum phraseRoles {
        TextRole = Qt::UserRole + 1,
        IdRole,
        SoundFileRole,
        ExcludedRole,
        DataRole
    };

    explicit PhraseModel(QObject *parent = 0);
    void setUnit(Unit *unit);
    Unit * unit() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /**
     * Count phrases in current model view. If this number is changed, signal
     * countChanged() is emitted.
     *
     * \return number of phrases
     */
    int count() const;

signals:
    void phraseChanged(int index);
    void unitChanged();
    void typeChanged();
    void countChanged();

private slots:
    void onPhraseAboutToBeAdded(Phrase *unit, int index);
    void onPhraseAdded();
    void onPhrasesAboutToBeRemoved(int first, int last);
    void onPhrasesRemoved();
    void emitPhraseChanged(int row);

private:
    void updateMappings();
    Unit *m_unit;
    QSignalMapper *m_signalMapper;
};

#endif // PHRASEMODEL_H
