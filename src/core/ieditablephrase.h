/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef IEDITABLEPHRASE_H
#define IEDITABLEPHRASE_H

#include "artikulatecore_export.h"
#include "iphrase.h"
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

    ~IEditablePhrase() override = default;

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
