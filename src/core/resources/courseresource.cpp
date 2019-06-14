/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Oindrila Gupta <oindrila.gupta92@gmail.com>
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

#include "courseresource.h"
#include "courseparser.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/language.h"
#include "core/iresourcerepository.h"

#include <memory>
#include <QQmlEngine>
#include <QXmlSchema>
#include <QXmlStreamReader>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include "artikulate_debug.h"

class CourseResourcePrivate
{
public:
    CourseResourcePrivate() = default;
    ~CourseResourcePrivate();

    void loadCourse(CourseResource *parent);

    IResourceRepository *m_repository{ nullptr };
    QUrl m_file;
    QString m_identifier;
    QString m_foreignId;
    QString m_title;
    QString m_languageId;
    std::shared_ptr<Language> m_language;
    QString m_i18nTitle;
    QString m_description;
    QVector<std::shared_ptr<Unit>> m_units;
    bool m_courseLoaded{ false }; ///<! indicates if course was completely parsed
};

CourseResourcePrivate::~CourseResourcePrivate() = default;

void CourseResourcePrivate::loadCourse(CourseResource *parent)
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
    auto units = CourseParser::parseUnits(m_file, phonemes);
    for (auto &unit : units) {
        parent->addUnit(std::move(unit));
    }
}

CourseResource::CourseResource(const QUrl &path, IResourceRepository *repository)
    : ICourse(repository)
    , d(new CourseResourcePrivate())
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    d->m_file = path;
    d->m_repository = repository;

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
            //TODO i18nTitle must be implemented, currently missing and hence not parsed
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
            if (!d->m_identifier.isEmpty()
                && !d->m_title.isEmpty()
                && !d->m_i18nTitle.isEmpty()
                && !d->m_description.isEmpty()
                && !d->m_languageId.isEmpty()
                && !d->m_foreignId.isEmpty()
            )
            {
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

std::shared_ptr<Language> CourseResource::language() const
{
    return d->m_language;
}

void CourseResource::setLanguage(std::shared_ptr<Language> language)
{
    if (d->m_language == language) {
        return;
    }
    d->m_language = language;
    emit languageChanged();
}

std::shared_ptr<Unit> CourseResource::addUnit(std::unique_ptr<Unit> unit)
{
    emit unitAboutToBeAdded(unit.get(), d->m_units.count() - 1);
    d->m_units.append(std::move(unit));
    emit unitAdded();
    return d->m_units.last();
}

QList<Unit *> CourseResource::unitList()
{
    if (d->m_courseLoaded == false) {
        d->loadCourse(this);
    }
    QList<Unit *> rawList;
    for (auto unit : d->m_units) {
        rawList.append(unit.get());
    }
    return rawList;
}

QVector<std::shared_ptr<Unit>> CourseResource::units()
{
    if (d->m_courseLoaded == false) {
        d->loadCourse(this);
    }
    return d->m_units;
}

QUrl CourseResource::file() const
{
    return d->m_file;
}
