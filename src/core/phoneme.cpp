/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "phoneme.h"
#include "artikulate_debug.h"

Phoneme::Phoneme(QObject *parent)
    : QObject(parent)
{
}

QString Phoneme::id() const
{
    return m_id;
}

void Phoneme::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
    }
}

QString Phoneme::title() const
{
    return m_title;
}

void Phoneme::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
    }
}
