/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef RESOURCEREPOSITORY_H
#define RESOURCEREPOSITORY_H

#include "artikulatecore_export.h"
#include "iresourcerepository.h"
#include <QHash>
#include <QMap>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QVector>

class CourseResource;
class ICourse;
class Language;
class LanguageResource;

/**
 * @class ResourceRepository
 *
 * This class provides data handling of all downloaded trainingdata of a user. It dervies from the repository interface
 * to provide a generalized access to the data.
 */
class ARTIKULATECORE_EXPORT ResourceRepository : public IResourceRepository
{
    Q_OBJECT
    Q_INTERFACES(IResourceRepository)

public:
    explicit ResourceRepository();
    ~ResourceRepository() override;

    /**
     * @brief Construtor for ResourceRepository object with explicitly set course folder
     *
     * @param storageLocation relative or absolute path to courses/ folder (including that directory)
     */
    explicit ResourceRepository(const QUrl &storageLocation);

    /**
     * @return path to repository location
     */
    QUrl storageLocation() const override;

    /**
     * @return list of available courses
     */
    QVector<std::shared_ptr<ICourse>> courses() const override;

    /**
     * @return list of available courses
     */
    QVector<std::shared_ptr<ICourse>> courses(const QString &languageId) const override;

    /**
     * @return list of all available language specifications
     */
    QVector<std::shared_ptr<ILanguage>> languages() const override;

    std::shared_ptr<ILanguage> language(const QString &id) const;

public Q_SLOTS:
    /**
     * \brief updates available resources
     */
    void reloadCourses() override;

private:
    bool loadCourse(const QString &resourceFile);
    bool loadLanguage(const QString &resourceFile);
    QVector<std::shared_ptr<ICourse>> m_courses;
    QHash<QString, std::shared_ptr<ILanguage>> m_languages; ///>! (language-identifier, language resource)
    QStringList m_loadedCourses;
    const QUrl m_storageLocation;
};

#endif // RESOURCEREPOSITORY_H
