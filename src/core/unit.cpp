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

#include "unit.h"
#include "phrase.h"

#include <KDebug>
#include <KUrl>

Unit::Unit(QObject *parent)
    : QObject(parent)
{
}

QString Unit::id() const
{
    return m_id;
}

void Unit::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
        emit modified();
    }
}

QString Unit::title() const
{
    return m_title;
}

void Unit::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
        emit modified();
    }
}

QList< Phrase* > Unit::phraseList() const
{
    return m_phraseList;
}

void Unit::addPhrase(Phrase *phrase)
{
    QList<Phrase *>::ConstIterator iter = m_phraseList.constBegin();
    while (iter != m_phraseList.constEnd()) {
        if (phrase->id() == (*iter)->id()) {
            kWarning() << "Phrase is already contained in this unit, aborting";
            return;
        }
        ++iter;
    }
    m_phraseList.append(phrase);

    connect(phrase, SIGNAL(idChanged()), this, SIGNAL(modified()));
    connect(phrase, SIGNAL(textChanged()), this, SIGNAL(modified()));
    connect(phrase, SIGNAL(soundChanged()), this, SIGNAL(modified()));
    connect(phrase, SIGNAL(prononciationTagsChanged()), this, SIGNAL(modified()));

    emit modified();
}

