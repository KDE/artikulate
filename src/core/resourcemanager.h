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

#include "artikulatecore_export.h"
#include <QObject>
#include <QMap>

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
class ARTIKULATELIB_EXPORT ResourceManager : public QObject
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

    /**
     * \return list of all loaded courses for language \p language
     */
    QList<Course *> courseList(Language *language) const;

    Q_INVOKABLE Course * course(Language *language, int index) const;

    /**
     * Load course from locally stored XML file.
     * TODO allow loading of remote XML files
     *
     * \param courseFile is the local XML file containing the course
     * \return loaded course
     */
    Course * loadCourse(const KUrl &courseFile);

    /**
     * Reset the course from its course file.
     *
     * \param course the course to be reloaded
     */
    Q_INVOKABLE void reloadCourse(Course *course);

    /**
     * Adds course to resource manager by parsing the given course specification file.
     *
     * \param courseFile is the local XML file containing the course
     * \return true if loaded successfully, otherwise false
     */
    bool addCourse(const KUrl &courseFile);

    /**
     * Adds course to resource manager. If the course's langauge is not registered, the language
     * is registered by this method.
     *
     * \param course the course to add to resource manager
     */
    void addCourse(Course *course);

    /**
     * Remove course from resource manager. If the course is modified its changes are NOT
     * written. For writing changes, the Cousse::sync() method must be called directly.
     *
     * \param course is the course to be removed
     */
    void removeCourse(Course *course);

    Q_INVOKABLE void newCourseDialog();

    /**
     * Serializes course object to its file. The course must have a valid local file path set.
     *
     * \param course is the course to be serialized
     */
    static void sync(Course *course);

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
    QXmlSchema loadXmlSchema(const QString &schemeName) const;

    /**
     * Load XML file given by \p file that confirms with XML schema \p scheme.
     *
     * \param path is the path to the XML file to be loaded
     * \param scheme is the XML schema describing the DOM
     * \return the loaded DOM document
     */
    QDomDocument loadDomDocument(const KUrl &path, const QXmlSchema &schema) const;

    QList<Language *> m_languageList;
    QMap<Language *, QList<Course *> > m_courseList;
};

#endif // RESOURCEMANAGER_H