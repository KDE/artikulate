/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef IUNIT_H
#define IUNIT_H

#include "artikulatecore_export.h"
#include <QMap>
#include <QObject>
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
    void phraseAboutToBeAdded(std::shared_ptr<IPhrase>, int);
    void phraseAdded(std::shared_ptr<IPhrase>);
    void phraseAboutToBeRemoved(int);
    void phraseRemoved();

protected:
    IUnit(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    virtual void setSelf(std::shared_ptr<IUnit> unit) = 0;
};

Q_DECLARE_INTERFACE(IUnit, "com.kde.artikulate.IUnit/1.0")

#endif // IUNIT_H
