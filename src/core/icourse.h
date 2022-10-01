/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef ICOURSE_H
#define ICOURSE_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QUrl>
#include <memory>

class QString;
class ILanguage;
class IUnit;
class Unit;
class Phoneme;

class ARTIKULATECORE_EXPORT ICourse : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString i18nTitle READ i18nTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString languageTitle READ languageTitle CONSTANT)

public:
    ~ICourse() override = default;
    virtual QString id() const = 0;
    virtual QString foreignId() const = 0;
    virtual QString title() const = 0;
    virtual QString i18nTitle() const = 0;
    virtual QString description() const = 0;
    virtual QString languageTitle() const = 0;
    virtual std::shared_ptr<ILanguage> language() const = 0;
    /**
     * @brief Lazy loading unit list
     * @return list of units in course
     */
    virtual QVector<std::shared_ptr<Unit>> units() = 0;
    virtual QUrl file() const = 0;

protected:
    ICourse()
        : QObject()
    {
    }
    virtual void setSelf(std::shared_ptr<ICourse> course) = 0;

Q_SIGNALS:
    void idChanged();
    void titleChanged();
    void descriptionChanged();
    void languageChanged();
    void unitAdded();
    void unitAboutToBeAdded(std::shared_ptr<Unit> unit, int index);
    void unitsRemoved();
    void unitsAboutToBeRemoved(int, int);
};

Q_DECLARE_INTERFACE(ICourse, "com.kde.artikulate.ICourse/1.0")

#endif // COURSE_H
