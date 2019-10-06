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

#ifndef IPHRASE_H
#define IPHRASE_H

#include "artikulatecore_export.h"
#include <QMap>
#include <QObject>
#include <QUrl>
#include <memory>

class QString;
class IUnit;
class Phoneme;

class ARTIKULATECORE_EXPORT IPhrase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QString i18nText READ i18nText NOTIFY i18nTextChanged)
    Q_PROPERTY(QString soundFileUrl READ soundFileUrl NOTIFY soundChanged)
    Q_PROPERTY(IPhrase::Type type READ type NOTIFY typeChanged)

public:
    enum class Type { Word, Expression, Sentence, Paragraph, AllTypes };
    Q_ENUM(Type)

    virtual ~IPhrase() = default;

    virtual QString id() const = 0;
    virtual QString foreignId() const = 0;
    virtual QString text() const = 0;
    virtual QString i18nText() const = 0;
    virtual std::shared_ptr<IUnit> unit() const = 0;
    virtual IPhrase::Type type() const = 0;
    virtual QString typeString() const = 0;
    virtual QString soundFileUrl() const = 0;
    virtual QUrl sound() const = 0;
    virtual QVector<Phoneme *> phonemes() const = 0;
    virtual std::shared_ptr<IPhrase> self() const = 0;

protected:
    IPhrase()
        : QObject()
    {
    }
    virtual void setSelf(std::shared_ptr<IPhrase> phrase) = 0;

Q_SIGNALS:
    void idChanged();
    void unitChanged();
    void textChanged();
    void i18nTextChanged();
    void typeChanged();
    void soundChanged();
    void phonemesChanged();
};

Q_DECLARE_INTERFACE(IPhrase, "com.kde.artikulate.IPhrase/1.0")

#endif // IPHRASE_H
