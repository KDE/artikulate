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

#include "language.h"
#include "models/languagemodel.h"
#include "tag.h"
#include "taggroup.h"
#include <KDebug>

Language::Language(QObject *parent)
    : QObject(parent)
{
}

Language::~Language()
{
    qDeleteAll(m_prononciationTags);
}

QString Language::id() const
{
    return m_id;
}

void Language::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
    }
}

QString Language::title() const
{
    return m_title;
}

void Language::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
    }
}

KUrl Language::file() const
{
    return m_file;
}

void Language::setFile(const KUrl& file)
{
    m_file = file;
}

QList<Tag *> Language::prononciationTags() const
{
    return m_prononciationTags;
}

Tag * Language::addPrononciationTag(const QString &identifier, const QString &title)
{
    QList<Tag *>::ConstIterator iter = m_prononciationTags.constBegin();
    while (iter != m_prononciationTags.constEnd()) {
        if (QString::compare((*iter)->id(), identifier) == 0) {
            kWarning() << "Prononciation Tag identifier already registered, aborting";
            return 0;
        }
        ++iter;
    }

    Tag *newTag = new Tag();
    newTag->setId(identifier);
    newTag->setTitle(title);
    m_prononciationTags.append(newTag);
    emit tagsChanged();

    return newTag;
}

QList<TagGroup*> Language::prononciationGroups() const
{
    return m_prononciationGroups;
}

TagGroup * Language::addPrononciationGroup(const QString &identifier, const QString &title)
{
    QList<TagGroup *>::ConstIterator iter = m_prononciationGroups.constBegin();
    while (iter != m_prononciationGroups.constEnd()) {
        if (QString::compare((*iter)->id(), identifier) == 0) {
            kWarning() << "Prononciation Group identifier already registered, aborting";
            return 0;
        }
        ++iter;
    }

    TagGroup *newGroup = new TagGroup();
    newGroup->setId(identifier);
    newGroup->setTitle(title);
    m_prononciationGroups.append(newGroup);
    emit groupsChanged();

    return newGroup;
}
