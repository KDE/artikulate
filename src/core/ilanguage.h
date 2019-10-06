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

#ifndef ILANGUAGE_H
#define ILANGUAGE_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QUrl>
#include <QVector>
#include <memory>

class QString;
class Phoneme;
class PhonemeGroup;

class ARTIKULATECORE_EXPORT ILanguage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString i18nTitle READ i18nTitle NOTIFY i18nTitleChanged)

public:
    virtual ~ILanguage() = default;
    virtual QString id() const = 0;
    virtual QString title() const = 0;
    virtual QString i18nTitle() const = 0;
    virtual QVector<std::shared_ptr<Phoneme>> phonemes() const = 0;
    virtual QVector<std::shared_ptr<PhonemeGroup>> phonemeGroups() const = 0;

protected:
    ILanguage(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

Q_SIGNALS:
    void idChanged();
    void titleChanged();
    void i18nTitleChanged();
    void phonemesChanged();
    void phonemeGroupsChanged();
};
Q_DECLARE_INTERFACE(ILanguage, "com.kde.artikulate.ILanguage/1.0")

#endif // ILANGUAGE_H
