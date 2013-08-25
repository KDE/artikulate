/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "languageresource.h"
#include "../resourcemanager.h"
#include "../language.h"

class LanguageResourcePrivate
{
public:
    LanguageResourcePrivate(ResourceManager *resourceManager)
        : m_resourceManager(resourceManager)
        , m_languageResource(0)
    {
    }

    ~LanguageResourcePrivate()
    {
    }

    ResourceManager *m_resourceManager;
    KUrl m_path;
    ResourceInterface::Type m_type;
    QString m_identifier;
    QString m_title;
    Language *m_languageResource;
};

LanguageResource::LanguageResource(ResourceManager *resourceManager, const KUrl &path)
    : ResourceInterface(resourceManager)
    , d(new LanguageResourcePrivate(resourceManager))
{
    d->m_type = ResourceInterface::LanguageResource;
    d->m_path = path;

    //FIXME load basics
}

LanguageResource::~LanguageResource()
{

}

QString LanguageResource::identifier()
{
    return d->m_identifier;
}

QString LanguageResource::title()
{
    return d->m_title;
}


ResourceInterface::Type LanguageResource::type() const
{
    return d->m_type;
}


void LanguageResource::close()
{
    // do nothing
}

bool LanguageResource::isOpen() const
{
    return (d->m_languageResource != 0);
}

KUrl LanguageResource::path() const
{
    return d->m_path;
}

QObject * LanguageResource::resource()
{
    return d->m_languageResource;
}

