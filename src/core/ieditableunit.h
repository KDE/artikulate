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

#ifndef IEDITABLEUNIT_H
#define IEDITABLEUNIT_H

#include "artikulatecore_export.h"
#include "iunit.h"
#include <QMap>
#include <QUrl>
#include <memory>

class QString;
class ICourse;
class IPhrase;
class IEditablePhrase;
class Phoneme;

class ARTIKULATECORE_EXPORT IEditableUnit : public IUnit
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    virtual ~IEditableUnit() = default;
    virtual void setId(const QString &id) = 0;
    virtual void setForeignId(const QString &id) = 0;
    virtual void setCourse(std::shared_ptr<ICourse> course) = 0;
    virtual void setTitle(const QString &title) = 0;
    virtual void addPhrase(std::shared_ptr<IEditablePhrase> phrase) = 0;

Q_SIGNALS:
    void modified();

protected:
    IEditableUnit(QObject *parent = nullptr)
        : IUnit(parent)
    {
    }
};

Q_DECLARE_INTERFACE(IEditableUnit, "com.kde.artikulate.IEditableUnit/1.0")

#endif // IEDITABLEUNIT_H
