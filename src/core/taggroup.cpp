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

#include "taggroup.h"
#include "tag.h"

#include <KDebug>

TagGroup::TagGroup(QObject *parent)
    : QObject(parent)
{
}

QString TagGroup::id() const
{
    return m_id;
}

void TagGroup::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
    }
}

QString TagGroup::title() const
{
    return m_title;
}

void TagGroup::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
    }
}

QString TagGroup::description() const
{
    return m_description;
}

void TagGroup::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

QList< Tag* > TagGroup::tags() const
{
    return m_tags;
}

void TagGroup::addTag(Tag *tag)
{
    QList<Tag *>::ConstIterator iter = m_tags.constBegin();
    while (iter != m_tags.constEnd()) {
        if (QString::compare((*iter)->id(), tag->id()) == 0) {
            kWarning() << "Prononciation Tag identifier already registered, aborting";
            return;
        }
        ++iter;
    }
    m_tags.append(tag);
}

void TagGroup::removeTag(Tag *tag)
{
    m_tags.removeOne(tag);
}
