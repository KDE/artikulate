/*
 *  SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  SPDX-FileCopyrightText: 2013 Oindrila Gupta <oindrila.gupta92@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "courseresource.h"
#include "core/iresourcerepository.h"
#include "core/language.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/unit.h"
#include "courseparser.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QQmlEngine>
#include <QXmlSchema>
#include <QXmlStreamReader>
#include <memory>

#include "artikulate_debug.h"

class CourseResourcePrivate
{
public:
    CourseResourcePrivate() = default;
    ~CourseResourcePrivate();

    void loadCourse(CourseResource *parent, bool skipIncomplete);

    std::weak_ptr<ICourse> m_self;
    IResourceRepository *m_repository {nullptr};
    QUrl m_file;
    QString m_identifier;
    QString m_foreignId;
    QString m_title;
    QString m_languageId;
    std::shared_ptr<ILanguage> m_language;
    QString m_i18nTitle;
    QString m_description;
    QVector<std::shared_ptr<Unit>> m_units;
    bool m_courseLoaded {false}; ///<! indicates if course was completely parsed
    bool m_skipIncomplete {false};
};

CourseResourcePrivate::~CourseResourcePrivate() = default;

void CourseResourcePrivate::loadCourse(CourseResource *parent, bool skipIncomplete)
{
    if (m_courseLoaded == true) {
        qCWarning(ARTIKULATE_CORE()) << "Skipping loading of course, no reloading implemented yet";
        return;
    }
    m_courseLoaded = true;

    QFileInfo info(m_file.toLocalFile());
    if (!info.exists()) {
        qCCritical(ARTIKULATE_CORE()) << "No course file available at location" << m_file.toLocalFile();
        return;
    }

    QVector<std::shared_ptr<Phoneme>> phonemes = m_language->phonemes();
    auto units = CourseParser::parseUnits(m_file, phonemes, skipIncomplete);
    for (auto &unit : units) {
        if (!skipIncomplete || unit->phrases().count() > 0) {
            parent->addUnit(std::move(unit));
        }
    }
}

std::shared_ptr<CourseResource> CourseResource::create(const QUrl &path, IResourceRepository *repository, bool skipIncomplete)
{
    std::shared_ptr<CourseResource> course(new CourseResource(path, repository, skipIncomplete));
    course->setSelf(course);
    return course;
}

void CourseResource::setSelf(std::shared_ptr<ICourse> self)
{
    Q_ASSERT(d->m_self.expired());
    d->m_self = self;
}

std::shared_ptr<ICourse> CourseResource::self() const
{
    return d->m_self.lock();
}

CourseResource::CourseResource(const QUrl &path, IResourceRepository *repository, bool skipIncomplete)
    : ICourse()
    , d(new CourseResourcePrivate())
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    d->m_file = path;
    d->m_repository = repository;
    d->m_skipIncomplete = skipIncomplete;

    // load basic information from language file, but does not parse everything
    QXmlStreamReader xml;
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly)) {
        xml.setDevice(&file);
        xml.readNextStartElement();
        while (xml.readNext() && !xml.atEnd()) {
            if (xml.name() == "id") {
                d->m_identifier = xml.readElementText();
                continue;
            }
            if (xml.name() == "foreignId") {
                d->m_foreignId = xml.readElementText();
                continue;
            }
            // TODO i18nTitle must be implemented, currently missing and hence not parsed
            if (xml.name() == "title") {
                d->m_title = xml.readElementText();
                d->m_i18nTitle = d->m_title;
                continue;
            }
            if (xml.name() == "description") {
                d->m_description = xml.readElementText();
                continue;
            }
            if (xml.name() == "language") {
                d->m_languageId = xml.readElementText();
                continue;
            }

            // quit reading when basic elements are read
            if (!d->m_identifier.isEmpty() && !d->m_title.isEmpty() && !d->m_i18nTitle.isEmpty() && !d->m_description.isEmpty() && !d->m_languageId.isEmpty() && !d->m_foreignId.isEmpty()) {
                break;
            }
        }
        if (xml.hasError()) {
            qCritical() << "Error occurred when reading Course XML file:" << path.toLocalFile();
        }
    } else {
        qCCritical(ARTIKULATE_CORE()) << "Could not open course file" << path.toLocalFile();
    }
    xml.clear();
    file.close();

    // find correct language
    if (repository != nullptr) {
        for (const auto &language : repository->languages()) {
            if (language == nullptr) {
                continue;
            }
            if (language->id() == d->m_languageId) {
                d->m_language = language;
            }
        }
    }
    if (d->m_language == nullptr) {
        qCCritical(ARTIKULATE_CORE()) << "A course with an unknown language was loaded";
    }
}

CourseResource::~CourseResource() = default;

QString CourseResource::id() const
{
    return d->m_identifier;
}

void CourseResource::setId(const QString &id)
{
    if (d->m_identifier == id) {
        return;
    }
    d->m_identifier = id;
    emit idChanged();
}

QString CourseResource::foreignId() const
{
    return d->m_foreignId;
}

void CourseResource::setForeignId(const QString &foreignId)
{
    if (d->m_foreignId == foreignId) {
        return;
    }
    d->m_foreignId = foreignId;
    emit foreignIdChanged();
}

QString CourseResource::title() const
{
    return d->m_title;
}

void CourseResource::setTitle(const QString &title)
{
    if (d->m_title == title) {
        return;
    }
    d->m_title = title;
    emit titleChanged();
}

QString CourseResource::i18nTitle() const
{
    return d->m_i18nTitle;
}

void CourseResource::setI18nTitle(const QString &i18nTitle)
{
    if (d->m_i18nTitle == i18nTitle) {
        return;
    }
    d->m_i18nTitle = i18nTitle;
    emit i18nTitleChanged();
}

QString CourseResource::description() const
{
    return d->m_description;
}

void CourseResource::setDescription(const QString &description)
{
    if (d->m_description == description) {
        return;
    }
    d->m_description = description;
    emit descriptionChanged();
}

std::shared_ptr<ILanguage> CourseResource::language() const
{
    return d->m_language;
}

QString CourseResource::languageTitle() const
{
    if (d->m_language) {
        return d->m_language->title();
    }
    return QString();
}

void CourseResource::setLanguage(std::shared_ptr<ILanguage> language)
{
    if (d->m_language == language) {
        return;
    }
    d->m_language = language;
    emit languageChanged();
}

std::shared_ptr<Unit> CourseResource::addUnit(std::shared_ptr<Unit> unit)
{
    std::shared_ptr<Unit> storedUnit(std::move(unit));
    storedUnit->setCourse(self());
    emit unitAboutToBeAdded(storedUnit, d->m_units.count() - 1);
    d->m_units.append(storedUnit);
    emit unitAdded();
    return storedUnit;
}

QVector<std::shared_ptr<Unit>> CourseResource::units()
{
    if (d->m_courseLoaded == false) {
        d->loadCourse(this, d->m_skipIncomplete);
    }
    return d->m_units;
}

QUrl CourseResource::file() const
{
    return d->m_file;
}
