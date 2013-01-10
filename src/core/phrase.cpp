/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#include "phrase.h"

#include <KDebug>

Phrase::Phrase(QObject *parent)
    : QObject(parent)
{
}

QString Phrase::id() const
{
    return m_id;
}

void Phrase::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
    }
}

QString Phrase::text() const
{
    return m_text;
}

void Phrase::setText(const QString &text)
{
    if (QString::compare(text, m_text) != 0) {
        m_text = text;
        emit textChanged();
    }
}

KUrl Phrase::sound() const
{
    return m_sound;
}

void Phrase::setSound(const KUrl &soundFile)
{
    m_sound = soundFile;
    emit soundChanged();
}
