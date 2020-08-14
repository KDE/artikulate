/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef EDITABLECOURSERESOURCE_H
#define EDITABLECOURSERESOURCE_H

#include "artikulatecore_export.h"
#include "core/icourse.h"
#include "core/ieditablecourse.h"
#include "core/ieditableunit.h"
#include "courseresource.h"
#include <QObject>
#include <QVector>
#include <memory>

class IResourceRepository;
class Course;
class Unit;
class IPhrase;
class QString;

/**
 * @brief Decorator for CourseResource
 *
 * This decorator adds functionality to modify and write back changes of a course.
 */
class ARTIKULATECORE_EXPORT EditableCourseResource : public IEditableCourse
{
    Q_OBJECT
    Q_INTERFACES(ICourse)
    Q_INTERFACES(IEditableCourse)

public:
    static std::shared_ptr<EditableCourseResource> create(const QUrl &path, IResourceRepository *repository);
    ~EditableCourseResource() override = default;

    /**
     * \return unique identifier
     */
    QString id() const override;

    void setId(QString id) override;

    /**
     * \return unique identifier
     */
    QString foreignId() const override;

    void setForeignId(QString foreignId) override;

    /**
     * \return human readable localized title
     */
    QString title() const override;

    void setTitle(QString title) override;

    /**
     * \return human readable title in English
     */
    QString i18nTitle() const override;

    void setI18nTitle(QString i18nTitle) override;

    /**
     * \return description text for course
     */
    QString description() const override;

    void setDescription(QString description) override;
    /**
     * \return language identifier of this course
     */
    std::shared_ptr<ILanguage> language() const override;
    QString languageTitle() const override;
    void setLanguage(std::shared_ptr<ILanguage> language) override;
    bool sync() override;
    bool exportToFile(const QUrl &filePath) const override;

    std::shared_ptr<Unit> addUnit(std::shared_ptr<Unit> unit) override;
    QVector<std::shared_ptr<Unit>> units() override;
    void updateFrom(std::shared_ptr<ICourse> course) override;
    bool isModified() const override;
    QUrl file() const override;
    std::shared_ptr<IEditableCourse> self() const override;

    Q_INVOKABLE Unit *createUnit();
    Q_INVOKABLE bool createPhraseAfter(IPhrase *previousPhrase) override;
    Q_INVOKABLE bool deletePhrase(IPhrase *phrase) override;

Q_SIGNALS:
    void idChanged();
    void foreignIdChanged();
    void titleChanged();
    void i18nTitleChanged();
    void descriptionChanged();
    void languageChanged();

private Q_SLOTS:
    void markModified();

private:
    Q_DISABLE_COPY(EditableCourseResource)
    /**
     * Create course resource from file.
     */
    explicit EditableCourseResource(const QUrl &path, IResourceRepository *repository);
    void setSelf(std::shared_ptr<ICourse> self) override;
    mutable bool m_unitsLoaded {false}; ///< parsing of all units is postponed until needed, this variable indicates if they are read
    bool m_modified {false};
    const std::unique_ptr<CourseResource> m_course;
};

#endif
