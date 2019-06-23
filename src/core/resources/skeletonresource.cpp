/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "skeletonresource.h"
#include "courseparser.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "editablecourseresource.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/resources/languageresource.h"

#include <QDir>
#include <QQmlEngine>
#include <QDomDocument>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>

#include "artikulate_debug.h"

class SkeletonResourcePrivate
{
public:
    SkeletonResourcePrivate(const QUrl &path)
        : m_path(path)
    {
        // load basic information from language file, but does not parse everything
        QXmlStreamReader xml;
        QFile file(path.toLocalFile());
        if (file.open(QIODevice::ReadOnly)) {
            xml.setDevice(&file);
            xml.readNextStartElement();
            while (xml.readNext() && !xml.atEnd()) {
                if (xml.name() == "id") {
                    m_identifier = xml.readElementText();
                    continue;
                }
                if (xml.name() == "title") {
                    m_title = xml.readElementText();
                    continue;
                }
                if (xml.name() == "description") {
                    m_description = xml.readElementText();
                    continue;
                }

                // quit reading when basic elements are read
                if (!m_identifier.isEmpty()
                    && !m_title.isEmpty()
                    && !m_description.isEmpty()
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
    }

    QVector<std::shared_ptr<Unit>> units();

    std::shared_ptr<Unit> appendUnit(std::shared_ptr<Unit> unit);

    /**
     * @return the skeleton resource as serialized byte array
     */
    QDomDocument serializedSkeleton();

    std::weak_ptr<ICourse> m_self;
    QUrl m_path;
    QString m_identifier;
    QString m_title;
    QString m_description;
    bool m_unitsParsed{ false };

protected:
    QVector<std::shared_ptr<Unit>> m_units; ///!< the units variable is loaded lazily and shall never be access directly
};

QVector<std::shared_ptr<Unit>> SkeletonResourcePrivate::units()
{
    if (m_unitsParsed) {
        return m_units;
    }
    auto units = CourseParser::parseUnits(m_path);
    for (auto &unit : units) {
        m_units.append(std::move(unit));
    }
    m_unitsParsed = true;
    return m_units;
}

std::shared_ptr<Unit> SkeletonResourcePrivate::appendUnit(std::shared_ptr<Unit> unit) {
    units(); // ensure that units are parsed
    m_units.append(unit);
    return m_units.last();
}

QDomDocument SkeletonResourcePrivate::serializedSkeleton()
{
    QDomDocument document;
    // prepare xml header
    QDomProcessingInstruction header = document.createProcessingInstruction(QStringLiteral("xml"), QStringLiteral("version=\"1.0\""));
    document.appendChild(header);

    // create main element
    QDomElement root = document.createElement(QStringLiteral("skeleton"));
    document.appendChild(root);

    QDomElement idElement = document.createElement(QStringLiteral("id"));
    QDomElement titleElement = document.createElement(QStringLiteral("title"));
    QDomElement descriptionElement = document.createElement(QStringLiteral("description"));

    idElement.appendChild(document.createTextNode(m_identifier));
    titleElement.appendChild(document.createTextNode(m_title));
    descriptionElement.appendChild(document.createTextNode(m_description));

    QDomElement unitListElement = document.createElement(QStringLiteral("units"));
    // create units
    for (auto unit : units()) {
        QDomElement unitElement = document.createElement(QStringLiteral("unit"));

        QDomElement unitIdElement = document.createElement(QStringLiteral("id"));
        QDomElement unitTitleElement = document.createElement(QStringLiteral("title"));
        QDomElement unitPhraseListElement = document.createElement(QStringLiteral("phrases"));
        unitIdElement.appendChild(document.createTextNode(unit->id()));
        unitTitleElement.appendChild(document.createTextNode(unit->title()));

        // construct phrases
        for (Phrase *phrase : unit->phraseList()) {
            QDomElement phraseElement = document.createElement(QStringLiteral("phrase"));
            QDomElement phraseIdElement = document.createElement(QStringLiteral("id"));
            QDomElement phraseTextElement = document.createElement(QStringLiteral("text"));
            QDomElement phraseTypeElement = document.createElement(QStringLiteral("type"));

            phraseIdElement.appendChild(document.createTextNode(phrase->id()));
            phraseTextElement.appendChild(document.createTextNode(phrase->text()));
            phraseTypeElement.appendChild(document.createTextNode(phrase->typeString()));

            phraseElement.appendChild(phraseIdElement);
            phraseElement.appendChild(phraseTextElement);
            phraseElement.appendChild(phraseTypeElement);

            unitPhraseListElement.appendChild(phraseElement);
        }

        // construct the unit element
        unitElement.appendChild(unitIdElement);
        unitElement.appendChild(unitTitleElement);
        unitElement.appendChild(unitPhraseListElement);

        unitListElement.appendChild(unitElement);
    }

    root.appendChild(idElement);
    root.appendChild(titleElement);
    root.appendChild(descriptionElement);
    root.appendChild(unitListElement);

    return document;
}

std::shared_ptr<SkeletonResource> SkeletonResource::create(const QUrl &path, IResourceRepository *repository)
{
    std::shared_ptr<SkeletonResource> course(new SkeletonResource(path, repository));
    course->setSelf(course);
    return course;
}

SkeletonResource::SkeletonResource(const QUrl &path, IResourceRepository *repository)
    : IEditableCourse()
    , d(new SkeletonResourcePrivate(path))
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    Q_UNUSED(repository);
}

SkeletonResource::~SkeletonResource() = default;

void SkeletonResource::setSelf(std::shared_ptr<ICourse> self)
{
    d->m_self = self;
}

QString SkeletonResource::id() const
{
    return d->m_identifier;
}

void SkeletonResource::setId(QString id)
{
    if (d->m_identifier == id) {
        return;
    }
    d->m_identifier = id;
    emit idChanged();
}

QString SkeletonResource::foreignId() const
{
    return id();
}

void SkeletonResource::setForeignId(QString id)
{
    Q_UNUSED(id);
    Q_UNREACHABLE();
}

QString SkeletonResource::title() const
{
    return d->m_title;
}

void SkeletonResource::setTitle(QString title)
{
    if (d->m_title == title) {
        return;
    }
    d->m_title = title;
    emit titleChanged();
}

QString SkeletonResource::i18nTitle() const
{
    // there are no localized titles available
    return title();
}

void SkeletonResource::setI18nTitle(QString title)
{
    Q_UNUSED(title);
    Q_UNREACHABLE();
}

QString SkeletonResource::description() const
{
    return d->m_description;
}

void SkeletonResource::setDescription(QString description)
{
    if (d->m_description == description) {
        return;
    }
    d->m_description = description;
    emit descriptionChanged();
}

bool SkeletonResource::exportToFile(const QUrl &filePath) const
{
    // write back to file
    // create directories if necessary
    QFileInfo info(filePath.adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    if (!info.exists()) {
        qCDebug(ARTIKULATE_LOG()) << "create xml output file directory, not existing";
        QDir dir;
        dir.mkpath(filePath.adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    }

    //TODO port to atomic file swap
    QFile file(filePath.toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG()) << "Unable to open file " << filePath << " in write mode, aborting.";
        return false;
    }
    file.write(d->serializedSkeleton().toByteArray());

    return true;
}

std::shared_ptr<Unit> SkeletonResource::addUnit(std::unique_ptr<Unit> unit)
{
    std::shared_ptr<Unit> storedUnit(std::move(unit));
    emit unitAboutToBeAdded(storedUnit, d->units().count() - 1);
    d->appendUnit(storedUnit);
    emit unitAdded();
    return storedUnit;
}

bool SkeletonResource::isModified() const
{
    return false; //FIXME
}

std::shared_ptr<Language> SkeletonResource::language() const
{
    // skeleton must not have a dedicated language
    return std::shared_ptr<Language>();
}

void SkeletonResource::setLanguage(std::shared_ptr<Language> language)
{
    Q_UNUSED(language);
    Q_UNREACHABLE();
}

QVector<std::shared_ptr<Unit>> SkeletonResource::units()
{
    return d->units();
}

QUrl SkeletonResource::file() const
{
    return d->m_path;
}
