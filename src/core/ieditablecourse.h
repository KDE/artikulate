/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
    Q_PROPERTY(QString modified READ isModified NOTIFY modifiedChanged)

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
    Q_INVOKABLE virtual bool sync() = 0;
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
    void modifiedChanged(bool modified);

protected:
    IEditableCourse()
        : ICourse()
    {
    }
};

Q_DECLARE_INTERFACE(IEditableCourse, "com.kde.artikulate.IEditableCourse/1.0")

#endif // EDITABLECOURSE_H
