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

#ifndef RESOURCEINTERFACE_H
#define RESOURCEINTERFACE_H

#include <QObject>

class ResourceManager;
class KUrl;
class QXmlSchema;
class QDomDocument;

class ResourceInterface : public QObject
{
    Q_OBJECT

public:
    enum Type
    {
        LanguageResourceType,
        CourseResourceType,
        SkeletonResourceType
    };

    ResourceInterface(ResourceManager *resourceManager);

    virtual ~ResourceInterface();

    /**
     * \return unique identifier
     */
    virtual QString identifier() = 0;

    /**
     * \return human readable localized title
     */
    virtual QString title() = 0;

    /**
     * \return human readable title in English
     */
    virtual QString i18nTitle() = 0;

    /**
     * \return type of resource
     */
    virtual Type type() const = 0;

    /**
     * \return true if resource is loaded, otherwise false
     */
    virtual bool isOpen() const = 0;

    /**
     * close resource without writing changes back to file
     */
    virtual void close() = 0;

    /**
     * \return path to resource file
     */
    virtual KUrl path() const = 0;

    /**
     * Write changes to resource back to file.
     * This operation does _not_ close the file.
     */
    virtual void sync();

    /**
     * \return reference to the loaded resource
     * if resource is not open yet, it will be loaded
     */
    virtual QObject * resource() = 0;

    /**
     * Load XSD file given by its file name (without ".xsd" suffix). The method searches exclusively
     * the standard install dir for XSD files in subdirectory "schemes/".
     *
     * \param schemeName name of the Xml schema without suffix
     * \return loaded XML Schema
     */
    QXmlSchema loadXmlSchema(const QString &schemeName) const;

    /**
     * Load XML file given by \p file that confirms with XML schema \p scheme.
     *
     * \param path is the path to the XML file to be loaded
     * \param scheme is the XML schema describing the DOM
     * \return the loaded DOM document
     */
    QDomDocument loadDomDocument(const KUrl &path, const QXmlSchema &schema) const;
};


#endif
