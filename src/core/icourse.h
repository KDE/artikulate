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

#ifndef ICOURSE_H
#define ICOURSE_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QMap>
#include <QUrl>
#include <memory>

class QString;
class Language;
class Unit;

class ARTIKULATECORE_EXPORT ICourse : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString i18nTitle READ i18nTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)

public:
    ICourse(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    virtual ~ICourse() = default;
    virtual QString id() const = 0;
    virtual QString foreignId() const = 0;
    virtual QString title() const = 0;
    virtual QString i18nTitle() const = 0;
    virtual QString description() const = 0;
    virtual std::shared_ptr<Language> language() const = 0;
    /**
     * @brief Lazy loading unit list
     * @return list of units in course
     */
    virtual QList<Unit *> unitList() = 0;
    virtual QUrl file() const = 0;

Q_SIGNALS:
    void idChanged();
    void titleChanged();
    void descriptionChanged();
    void languageChanged();
    void unitAdded();
    void unitAboutToBeAdded(Unit*,int);
    void unitsRemoved();
    void unitsAboutToBeRemoved(int,int);
};

Q_DECLARE_INTERFACE(ICourse, "com.kde.artikulate.ICourse/1.0")

#endif // COURSE_H
