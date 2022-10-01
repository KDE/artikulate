/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PHONEME_H
#define PHONEME_H

#include "artikulatecore_export.h"
#include <QObject>

class QString;

/**
 * \class Phoneme
 * Phonemes are properties that can be assigned to a Phrase to specify its pronunciation characteristics.
 */
class ARTIKULATECORE_EXPORT Phoneme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    explicit Phoneme(QObject *parent = nullptr);
    QString id() const;
    void setId(const QString &id);
    QString title() const;
    void setTitle(const QString &title);

signals:
    void idChanged();
    void titleChanged();

private:
    Q_DISABLE_COPY(Phoneme)
    QString m_id;
    QString m_title;
};

#endif // PHONEME_H
