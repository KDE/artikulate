/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef IUNIT_H
#define IUNIT_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QMap>
#include <QUrl>
#include <memory>

class QString;
class ICourse;
class IPhrase;
class Phoneme;

class ARTIKULATECORE_EXPORT IUnit : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
    virtual ~IUnit() = default;
    virtual QString id() const = 0;
    virtual QString foreignId() const = 0;
    virtual std::shared_ptr<ICourse> course() const = 0;
    virtual QString title() const = 0;
    virtual QVector<std::shared_ptr<IPhrase>> phrases() const = 0;
    virtual std::shared_ptr<IUnit> self() const = 0;

Q_SIGNALS:
    void idChanged();
    void titleChanged();
    void courseChanged();
    void displayPhraseTypeChanged();
    void modified();
    void phraseAdded(std::shared_ptr<IPhrase>);
    void phraseAboutToBeAdded(std::shared_ptr<IPhrase>,int);
    void phraseRemoved(std::shared_ptr<IPhrase>);
    void phraseAboutToBeRemoved(int,int);

protected:
    IUnit(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    virtual void setSelf(std::shared_ptr<IUnit> unit) = 0;
};

Q_DECLARE_INTERFACE(IUnit, "com.kde.artikulate.IUnit/1.0")

#endif // IUNIT_H
