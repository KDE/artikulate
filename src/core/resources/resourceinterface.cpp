/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "resourceinterface.h"

#include "artikulate_debug.h"
#include <QUrl>
#include <QIODevice>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>
#include <QFile>
#include <QStandardPaths>

ResourceInterface::ResourceInterface(ResourceManager *resourceManager)
    : m_contributorResource(false)
{
    Q_UNUSED(resourceManager)
}

ResourceInterface::~ResourceInterface()
{

}

void ResourceInterface::setContributorResource(bool contributorResource)
{
    m_contributorResource = contributorResource;
}

bool ResourceInterface::isContributorResource() const
{
    return m_contributorResource;
}

void ResourceInterface::sync()
{
    qCWarning(ARTIKULATE_LOG) << "Resource does not implement syncing.";
}

void ResourceInterface::reload()
{
    qCWarning(ARTIKULATE_LOG) << "Resource does not implement reloading.";
}

QXmlSchema ResourceInterface::loadXmlSchema(const QString &schemeName) const
{
    QString relPath = QStringLiteral(":/artikulate/schemes/%1.xsd").arg(schemeName);
    QUrl file = QUrl::fromLocalFile(relPath);

    QXmlSchema schema;
    if (file.isEmpty() || schema.load(file) == false) {
        qCWarning(ARTIKULATE_LOG) << "Schema at file " << file.toLocalFile() << " is invalid.";
    }
    return schema;
}

QDomDocument ResourceInterface::loadDomDocument(const QUrl &path, const QXmlSchema &schema) const
{
    QDomDocument document;
    QXmlSchemaValidator validator(schema);
    if (!validator.validate(path)) {
        qCWarning(ARTIKULATE_LOG) << "Schema is not valid, aborting loading of XML document:" << path.toLocalFile();
        return document;
    }

    QString errorMsg;
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly)) {
        if (!document.setContent(&file, &errorMsg)) {
            qCWarning(ARTIKULATE_LOG) << errorMsg;
        }
    } else {
        qCWarning(ARTIKULATE_LOG) << "Could not open XML document " << path.toLocalFile() << " for reading, aborting.";
    }
    return document;
}
