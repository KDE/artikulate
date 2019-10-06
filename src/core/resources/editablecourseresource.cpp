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

#include "editablecourseresource.h"
#include "artikulate_debug.h"
#include "core/phoneme.h"
#include "core/phrase.h"
#include "core/unit.h"
#include "courseparser.h"
#include <KLocalizedString>
#include <KTar>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QQmlEngine>
#include <QUuid>
#include <memory>

EditableCourseResource::EditableCourseResource(const QUrl &path, IResourceRepository *repository)
    : IEditableCourse()
    , m_course(new CourseResource(path, repository))
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    connect(m_course.get(), &ICourse::unitAboutToBeAdded, this, &ICourse::unitAboutToBeAdded);
    connect(m_course.get(), &ICourse::unitAdded, this, &ICourse::unitAdded);
    connect(m_course.get(), &CourseResource::idChanged, this, &EditableCourseResource::idChanged);
    connect(m_course.get(), &CourseResource::foreignIdChanged, this, &EditableCourseResource::foreignIdChanged);
    connect(m_course.get(), &CourseResource::titleChanged, this, &EditableCourseResource::titleChanged);
    connect(m_course.get(), &CourseResource::descriptionChanged, this, &EditableCourseResource::descriptionChanged);
    connect(m_course.get(), &CourseResource::languageChanged, this, &EditableCourseResource::languageChanged);
}

std::shared_ptr<EditableCourseResource> EditableCourseResource::create(const QUrl &path, IResourceRepository *repository)
{
    std::shared_ptr<EditableCourseResource> course(new EditableCourseResource(path, repository));
    course->setSelf(course);
    return course;
}

void EditableCourseResource::setSelf(std::shared_ptr<ICourse> self)
{
    m_course->setSelf(self);
}

QString EditableCourseResource::id() const
{
    return m_course->id();
}

void EditableCourseResource::setId(QString id)
{
    if (m_course->id() != id) {
        m_course->setId(id);
        m_modified = true;
    }
}

QString EditableCourseResource::foreignId() const
{
    return m_course->foreignId();
}

void EditableCourseResource::setForeignId(QString foreignId)
{
    m_course->setForeignId(std::move(foreignId));
}

QString EditableCourseResource::title() const
{
    return m_course->title();
}

void EditableCourseResource::setTitle(QString title)
{
    if (m_course->title() != title) {
        m_course->setTitle(title);
        m_modified = true;
    }
}

QString EditableCourseResource::i18nTitle() const
{
    return m_course->i18nTitle();
}

void EditableCourseResource::setI18nTitle(QString i18nTitle)
{
    if (m_course->i18nTitle() != i18nTitle) {
        m_course->setI18nTitle(i18nTitle);
        m_modified = true;
    }
}

QString EditableCourseResource::description() const
{
    return m_course->description();
}

void EditableCourseResource::setDescription(QString description)
{
    if (m_course->description() != description) {
        m_course->setDescription(description);
        m_modified = true;
    }
}

std::shared_ptr<ILanguage> EditableCourseResource::language() const
{
    return m_course->language();
}

QString EditableCourseResource::languageTitle() const
{
    return m_course->languageTitle();
}

void EditableCourseResource::setLanguage(std::shared_ptr<ILanguage> language)
{
    if (m_course->language() != language) {
        m_course->setLanguage(language);
        m_modified = true;
    }
}

QUrl EditableCourseResource::file() const
{
    return m_course->file();
}

std::shared_ptr<IEditableCourse> EditableCourseResource::self() const
{
    return std::static_pointer_cast<IEditableCourse>(m_course->self());
}

bool EditableCourseResource::sync()
{
    Q_ASSERT(file().isValid());
    Q_ASSERT(file().isLocalFile());
    Q_ASSERT(!file().isEmpty());

    // not writing back if not modified
    if (!m_modified) {
        qCDebug(ARTIKULATE_LOG()) << "Aborting sync, course was not modified.";
        return false;
    }
    bool ok = exportToFile(file());
    if (ok) {
        m_modified = false;
    }
    return ok;
}

bool EditableCourseResource::exportToFile(const QUrl &filePath) const
{
    // write back to file
    // create directories if necessary
    QFileInfo info(filePath.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash).path());
    if (!info.exists()) {
        qCDebug(ARTIKULATE_LOG()) << "create xml output file directory, not existing";
        QDir dir;
        dir.mkpath(filePath.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash).path());
    }

    // TODO port to KSaveFile
    QFile file(filePath.toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG()) << "Unable to open file " << file.fileName() << " in write mode, aborting.";
        return false;
    }

    file.write(CourseParser::serializedDocument(self(), false).toByteArray());
    return true;
}

std::shared_ptr<Unit> EditableCourseResource::addUnit(std::shared_ptr<Unit> unit)
{
    m_modified = true;
    auto sharedUnit = m_course->addUnit(std::move(unit));
    sharedUnit->setCourse(self());
    return sharedUnit;
}

QVector<std::shared_ptr<Unit>> EditableCourseResource::units()
{
    if (!m_unitsLoaded) {
        for (auto &unit : m_course->units()) {
            unit->setCourse(self());
        }
        m_unitsLoaded = true;
    }
    return m_course->units();
}

void EditableCourseResource::updateFrom(std::shared_ptr<ICourse> skeleton)
{
    for (auto skeletonUnit : skeleton->units()) {
        // find matching unit or create one
        std::shared_ptr<Unit> matchingUnit;
        auto it = std::find_if(m_course->units().cbegin(), m_course->units().cend(), [skeletonUnit](std::shared_ptr<Unit> compareUnit) { return compareUnit->foreignId() == skeletonUnit->id(); });
        if (it == m_course->units().cend()) {
            // import complete unit
            auto importUnit = Unit::create();
            importUnit->setId(skeletonUnit->id());
            importUnit->setForeignId(skeletonUnit->id());
            importUnit->setTitle(skeletonUnit->title());
            matchingUnit = m_course->addUnit(std::move(importUnit));
        } else {
            matchingUnit = *it;
        }

        // import phrases
        for (auto skeletonPhrase : skeletonUnit->phrases()) {
            auto it = std::find_if(matchingUnit->phrases().cbegin(), matchingUnit->phrases().cend(), [skeletonPhrase](std::shared_ptr<IPhrase> comparePhrase) { return comparePhrase->foreignId() == skeletonPhrase->id(); });
            if (it == matchingUnit->phrases().cend()) {
                // import complete Phrase
                std::shared_ptr<Phrase> importPhrase = Phrase::create();
                importPhrase->setId(skeletonPhrase->id());
                importPhrase->setForeignId(skeletonPhrase->id());
                importPhrase->setText(skeletonPhrase->text());
                importPhrase->seti18nText(skeletonPhrase->i18nText());
                importPhrase->setType(skeletonPhrase->type());
                importPhrase->setUnit(matchingUnit);
                matchingUnit->addPhrase(importPhrase);
            }
        }
    }

    qCInfo(ARTIKULATE_LOG()) << "Update performed!";
}

bool EditableCourseResource::isModified() const
{
    return m_modified;
}

Unit *EditableCourseResource::createUnit()
{
    // find first unused id
    QStringList unitIds;
    for (auto unit : m_course->units()) {
        unitIds.append(unit->id());
    }
    QString id = QUuid::createUuid().toString();
    while (unitIds.contains(id)) {
        id = QUuid::createUuid().toString();
        qCWarning(ARTIKULATE_LOG) << "Unit id generator has found a collision, recreating id.";
    }

    // create unit
    std::shared_ptr<Unit> unit = Unit::create();
    unit->setCourse(self());
    unit->setId(id);
    unit->setTitle(i18n("New Unit"));
    auto sharedUnit = addUnit(std::move(unit));

    return sharedUnit.get();
}

std::shared_ptr<Phrase> EditableCourseResource::createPhrase(Unit *unit)
{
    // find globally unique phrase id inside course
    QStringList phraseIds;
    for (auto unit : m_course->units()) {
        for (auto &phrase : unit->phrases()) {
            phraseIds.append(phrase->id());
        }
    }
    QString id = QUuid::createUuid().toString();
    while (phraseIds.contains(id)) {
        id = QUuid::createUuid().toString();
        qCWarning(ARTIKULATE_LOG) << "Phrase id generator has found a collision, recreating id.";
    }

    // create unit
    std::shared_ptr<Phrase> phrase = Phrase::create();
    phrase->setId(id);
    phrase->setText(QLatin1String(""));
    phrase->setType(IPhrase::Type::Word);

    unit->addPhrase(phrase);

    return phrase;
}
