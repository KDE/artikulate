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
class Course;
class KUrl;
class QDomDocument;
class QFile;
class QXmlSchema;

/**
 * \class ResourceManager
 * This class loads and stores all data files of the application.
 */
class ResourceManager : public QObject
{
    Q_OBJECT
public:
    explicit ResourceManager(QObject *parent = 0);

    /**
     * This method loads all language and course files that are provided in the standard directories
     * for this application.
     */
    void loadLocalData();

    /**
     * \return list of all loaded language specifications
     */
    QList<Language *> languageList() const;

    Q_INVOKABLE Language * language(int index) const;

    /**
     * Load language specification from locally stored XML file.
     * TODO allow loading of remote XML files
     *
     * \param path is the local XML file containing the language specification
     * \return true if loaded successfully, otherwise false
     */
    bool loadLanguage(const KUrl &path);

    /**
     * \return list of all loaded courses
     */
    QList<Course *> courseList() const;

    Q_INVOKABLE Course * course(int index) const;

    /**
     * Load course from locally stored XML file.
     * TODO allow loading of remote XML files
     *
     * \param path is the local XML file containing the course
     * \return true if loaded successfully, otherwise false
     */
    bool loadCourse(const KUrl &path);

signals:
    void languageAdded();
    void languageAboutToBeAdded(Language*,int);
    void languageRemoved();
    void languageAboutToBeRemoved(int,int);
    void courseAdded();
    void courseAboutToBeAdded(Course*,int);
    void courseRemoved();
    void courseAboutToBeRemoved(int,int);

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
     * \param path is the path to the XML file to be loaded
     * \param scheme is the XML schema describing the DOM
     * \return the loaded DOM document
     */
    QDomDocument loadDomDocument(const KUrl &path, const QXmlSchema &schema);

    QList<Language *> m_languageList;
    QList<Course *> m_courseList;
};

#endif // RESOURCEMANAGER_H