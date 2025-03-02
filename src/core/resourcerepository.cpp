/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "resourcerepository.h"
#include "artikulate_debug.h"
#include "core/language.h"
#include "resources/courseresource.h"
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>

ResourceRepository::ResourceRepository()
    : ResourceRepository(
          QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).constFirst() + QStringLiteral("/courses/")))
{
}

ResourceRepository::ResourceRepository(const QUrl &storageLocation)
    : IResourceRepository()
    , m_storageLocation(storageLocation)
{
    qCDebug(ARTIKULATE_CORE()) << "Repository created from with location" << m_storageLocation;
    // load language resources
    // all other resources are only loaded on demand
    QDir dir(QStringLiteral(":/artikulate/languages/"));
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.completeSuffix() != QStringLiteral("xml")) {
            continue;
        }
        loadLanguage(fileInfo.absoluteFilePath());
    }
}

ResourceRepository::~ResourceRepository() = default;

QUrl ResourceRepository::storageLocation() const
{
    return m_storageLocation;
}

QVector<std::shared_ptr<ICourse>> ResourceRepository::courses() const
{
    QVector<std::shared_ptr<ICourse>> courses;
    for (const auto &course : m_courses) {
        courses.append(course);
    }
    return courses;
}

QVector<std::shared_ptr<ICourse>> ResourceRepository::courses(const QString &languageId) const
{
    QVector<std::shared_ptr<ICourse>> courses;
    for (const auto &course : m_courses) {
        if (course->language() && course->language()->id() == languageId) {
            continue;
        }
        courses.append(course);
    }
    return courses;
}

QVector<std::shared_ptr<ILanguage>> ResourceRepository::languages() const
{
    QVector<std::shared_ptr<ILanguage>> languages;
    for (const auto &language : m_languages) {
        if (language == nullptr) {
            continue;
        }
        languages.append(language);
    }
    return languages;
}

std::shared_ptr<ILanguage> ResourceRepository::language(const QString &id) const
{
    if (m_languages.contains(id)) {
        return m_languages.value(id);
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

    QDir rootDirectory = QDir(m_storageLocation.toLocalFile());
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

    auto resource = CourseResource::create(QUrl::fromLocalFile(resourceFile), this, true);
    if (resource->language() == nullptr) {
        qCCritical(ARTIKULATE_CORE()) << "Could not load course, language unknown:" << resourceFile;
        return false;
    }

    Q_EMIT courseAboutToBeAdded(resource, m_courses.count());
    m_courses.append(resource);
    Q_EMIT courseAdded();
    m_loadedCourses.append(resourceFile);
    return true;
}

bool ResourceRepository::loadLanguage(const QString &resourceFile)
{
    auto language = Language::create(QUrl::fromLocalFile(resourceFile));
    if (!language) {
        qCWarning(ARTIKULATE_CORE()) << "Could not load language" << resourceFile;
        return false;
    }
    if (m_languages.contains(language->id())) {
        qCWarning(ARTIKULATE_CORE()) << "Could not load language" << resourceFile;
        return false;
    }
    m_languages.insert(language->id(), language);
    return true;
}
