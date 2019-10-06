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

#ifndef IEDITABLEPHRASE_H
#define IEDITABLEPHRASE_H

#include "artikulatecore_export.h"
#include "iphrase.h"
#include <QMap>
#include <QObject>
#include <QUrl>
#include <memory>

class QString;
class Unit;
class Phoneme;

class ARTIKULATECORE_EXPORT IEditablePhrase : public IPhrase
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QString i18nText READ i18nText NOTIFY i18nTextChanged)
    Q_PROPERTY(QString soundFileUrl READ soundFileUrl NOTIFY soundChanged)
    Q_PROPERTY(IPhrase::Type type READ type NOTIFY typeChanged)

public:
    enum class EditState { Unknown, Translated, Completed };
    Q_ENUM(EditState)

    virtual ~IEditablePhrase() = default;

    virtual void setId(QString id) = 0;
    virtual void setForeignId(QString id) = 0;
    virtual void setText(QString text) = 0;
    virtual void seti18nText(QString text) = 0;
    virtual void setUnit(std::shared_ptr<IUnit> unit) = 0;
    virtual void setType(IPhrase::Type type) = 0;
    virtual void setSoundFileUrl() = 0; // TODO revisit as a setter should have an argument
    virtual IEditablePhrase::EditState editState() const = 0;
    virtual QString editStateString() const = 0;
    virtual void setEditState(IEditablePhrase::EditState state) = 0;
    virtual void setEditState(const QString &stateString) = 0;
    virtual void setSound(QUrl soundFile) = 0;

protected:
    IEditablePhrase()
        : IPhrase()
    {
    }

Q_SIGNALS:
    void modified();
    void editStateChanged();
};

Q_DECLARE_INTERFACE(IEditablePhrase, "com.kde.artikulate.IEditablePhrase/1.0")

#endif // IEDITABLEPHRASE_H
