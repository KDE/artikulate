/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef ILANGUAGE_H
#define ILANGUAGE_H

#include "artikulatecore_export.h"
#include <QObject>
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
    ~ILanguage() override = default;
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
