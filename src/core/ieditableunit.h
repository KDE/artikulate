/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
    virtual void setI18nTitle(const QString &title) = 0;
    virtual void addPhrase(std::shared_ptr<IEditablePhrase> phrase, int index) = 0;
    virtual void removePhrase(std::shared_ptr<IPhrase> phrase) = 0;

Q_SIGNALS:
    void modified();
    /**
     * @brief combines any change (rename, phrases)
     */
    void phrasesChanged(std::shared_ptr<IEditableUnit> unit);

protected:
    IEditableUnit(QObject *parent = nullptr)
        : IUnit(parent)
    {
    }
};

Q_DECLARE_INTERFACE(IEditableUnit, "com.kde.artikulate.IEditableUnit/1.0")

#endif // IEDITABLEUNIT_H
