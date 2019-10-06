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

#ifndef IEDITABLECOURSE_H
#define IEDITABLECOURSE_H

#include "artikulatecore_export.h"
#include "icourse.h"
#include <QObject>
#include <memory>

class QString;
class ILanguage;
class IEditableUnit;
class IPhrase;

class ARTIKULATECORE_EXPORT IEditableCourse : public ICourse
{
    Q_OBJECT
    Q_INTERFACES(ICourse)

public:
    virtual ~IEditableCourse() = default;
    virtual void setId(QString id) = 0;
    virtual void setForeignId(QString foreignId) = 0;
    virtual void setTitle(QString title) = 0;
    virtual void setI18nTitle(QString title) = 0;
    virtual void setDescription(QString description) = 0;
    virtual void setLanguage(std::shared_ptr<ILanguage> language) = 0;
    virtual std::shared_ptr<Unit> addUnit(std::shared_ptr<Unit> unit) = 0;
    Q_INVOKABLE virtual bool createPhraseAfter(IPhrase *phrase) = 0;
    Q_INVOKABLE virtual bool deletePhrase(IPhrase *phrase) = 0;
    /**
     * @brief Export course to specified file.
     * @param filePath the absolute path to the export file
     * @return true of export finished without errors
     */
    virtual bool exportToFile(const QUrl &filePath) const = 0;
    /**
     * @brief store editable course in file and set modified to false
     * @return true if no errors occurred
     */
    virtual bool sync() = 0;
    /**
     * @brief Update course from skeleton
     * This method imports all units and phrases from the specified skeleton
     *
     * @param skeleton
     */
    virtual void updateFrom(std::shared_ptr<ICourse> skeleton) = 0;
    virtual bool isModified() const = 0;
    virtual std::shared_ptr<IEditableCourse> self() const = 0;

Q_SIGNALS:
    void unitChanged(std::shared_ptr<IEditableUnit> unit);

protected:
    IEditableCourse()
        : ICourse()
    {
    }
};

Q_DECLARE_INTERFACE(IEditableCourse, "com.kde.artikulate.IEditableCourse/1.0")

#endif // EDITABLECOURSE_H
