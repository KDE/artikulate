/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#ifndef UNIT_H
#define UNIT_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QList>

class QString;
class Phrase;

class ARTIKULATELIB_EXPORT Unit : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    explicit Unit(QObject *parent = 0);

    QString id() const;
    void setId(const QString &id);
    QString title() const;
    void setTitle(const QString &title);
    QList<Phrase *> phraseList() const;
    void addPhrase(Phrase *phrase);

signals:
    void idChanged();
    void titleChanged();
    void modified();

private:
    Q_DISABLE_COPY(Unit)
    QString m_id;
    QString m_title;
    QList<Phrase *> m_phraseList;
};

#endif // UNIT_H
