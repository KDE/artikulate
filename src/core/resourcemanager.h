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


#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>

class Language;
class KUrl;
class QDomDocument;
class QFile;
class QXmlSchema;

class ResourceManager : public QObject
{
    Q_OBJECT
public:
    explicit ResourceManager(QObject *parent = 0);

    QList<Language *> languageList() const;
    bool loadLanguage(const KUrl &path);

private:
    /**
     * Load XSD file given by its file name (without ".xsd" suffix). The method searches exclusively
     * the standard install dir for XSD files in subdirectory "schemes/".
     *
     * \param schemeName name of the Xml schema without suffix
     * \return loaded XML Schema
     */
    QXmlSchema loadXmlSchema(const QString &schemeName);

    /**
     * Load XML file given by \p file that confirms with XML schema \p scheme.
     *
     * \param file is the file to be loaded
     * \param scheme is the XML schema describing the DOM
     * \return the loaded DOM document
     */
    QDomDocument loadDomDocument(const KUrl &file, const QXmlSchema &schema);

    QList<Language *> m_languageList;
};

#endif // RESOURCEMANAGER_H