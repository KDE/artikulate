/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "resourcerepository.h"
#include "artikulate_debug.h"
#include "resources/courseresource.h"
#include "resources/languageresource.h"
#include <QStandardPaths>
#include <QUrl>
#include <QDir>
#include <QDirIterator>

ResourceRepository::ResourceRepository(QObject *parent)
    : ResourceRepository(QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::DataLocation).constFirst() + QStringLiteral("/courses/")), parent)
{
}

ResourceRepository::ResourceRepository(const QUrl &storageLocation, QObject *parent)
    : IResourceRepository()
    , m_storageLocation(storageLocation.toLocalFile())
{
    // load language resources
    // all other resources are only loaded on demand
    QDir dir(":/artikulate/languages/");
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.completeSuffix() != QLatin1String("xml")) {
            continue;
        }
        loadLanguage(fileInfo.absoluteFilePath());
    }
}

QString ResourceRepository::storageLocation() const
{
    return m_storageLocation;
}

QVector<ICourse *> ResourceRepository::courses() const
{
    QVector<ICourse *> courses;
    for (const auto &course : m_courses) {
        courses.append(course);
    }
    return courses;
}

QVector<ICourse *> ResourceRepository::courses(Language *language) const
{
    QVector<ICourse *> courses;
    for (const auto &course : m_courses) {
        if (language != nullptr && course->language() != language) {
            continue;
        }
        courses.append(course);
    }
    return courses;
}

QVector<Language *> ResourceRepository::languages() const
{
    QVector<Language *> languages;
    for (const auto &language : m_languages) {
        if (language == nullptr) {
            continue;
        }
        languages.append(language->language());
    }
    return languages;
}

Language * ResourceRepository::language(const QString &id) const
{
    if (m_languages.contains(id)) {
        return m_languages.value(id)->language();
    }
    return nullptr;
}

void ResourceRepository::reloadCourses()
{
    std::function<void(QDir dir)> scanDirectoryForXmlCourseFiles = [this](QDir dir) {
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.completeSuffix() != QLatin1String("xml")) {
                continue;
            }
            loadCourse(fileInfo.absoluteFilePath());
        }
    };

    QDir rootDirectory = QDir(m_storageLocation);
    QDirIterator it(rootDirectory, QDirIterator::Subdirectories);
    qCInfo(ARTIKULATE_CORE()) << "Loading courses from" << rootDirectory.absolutePath();

    while (it.hasNext()) {
        scanDirectoryForXmlCourseFiles(it.next());
    }
}

bool ResourceRepository::loadCourse(const QString &resourceFile)
{
    qCDebug(ARTIKULATE_CORE()) << "Loading resource" << resourceFile;
    // skip already loaded resources
    if (m_loadedCourses.contains(resourceFile)) {
        qCWarning(ARTIKULATE_CORE()) << "Reloading of resources not yet supported, skippen course";
        return false;
    }

    CourseResource *resource = new CourseResource(QUrl::fromLocalFile(resourceFile), this);
    if (resource->language() == nullptr) {
        resource->deleteLater();
        qCCritical(ARTIKULATE_CORE()) << "Could not load course, language unknown:" << resourceFile;
        return false;
    }

    emit courseAboutToBeAdded(resource, m_courses.count() - 1);
    m_courses.append(resource);
    emit courseAdded();
    m_loadedCourses.append(resourceFile);
    return true;
}

bool ResourceRepository::loadLanguage(const QString &resourceFile)
{
    LanguageResource *resource = new LanguageResource(nullptr, QUrl::fromLocalFile(resourceFile)); //TODO hacky adapter code
    if (!resource) {
        qCWarning(ARTIKULATE_CORE()) << "Could not load language" << resourceFile;
        resource->deleteLater();
        return false;
    }
    if (m_languages.contains(resource->identifier())) {
        qCWarning(ARTIKULATE_CORE()) << "Could not load language" << resourceFile;
        resource->deleteLater();
        return false;
    }
    m_languages.insert(resource->identifier(), resource);
    return true;
}
