/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "skeletonresource.h"
#include "artikulate_debug.h"
#include "core/language.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/phrase.h"
#include "core/unit.h"
#include "courseparser.h"
#include "editablecourseresource.h"
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QQmlEngine>
#include <QUuid>
#include <QXmlStreamReader>

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
                if (!m_identifier.isEmpty() && !m_title.isEmpty() && !m_description.isEmpty()) {
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
        m_modified = false;
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
    bool m_unitsParsed {false};
    bool m_modified {false};

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
        Q_ASSERT(m_self.lock() != nullptr);
        unit->setCourse(m_self.lock());
        m_units.append(std::move(unit));
    }
    m_unitsParsed = true;
    return m_units;
}

std::shared_ptr<Unit> SkeletonResourcePrivate::appendUnit(std::shared_ptr<Unit> unit)
{
    units(); // ensure that units are parsed
    m_units.append(unit);
    m_modified = true;
    Q_ASSERT(m_self.lock() != nullptr);
    unit->setCourse(m_self.lock());
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
        for (auto &phrase : unit->phrases()) {
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
    connect(this, &SkeletonResource::idChanged, this, [=]() { d->m_modified = true; });
    connect(this, &SkeletonResource::titleChanged, this, [=]() { d->m_modified = true; });
    connect(this, &SkeletonResource::descriptionChanged, this, [=]() { d->m_modified = true; });

    Q_UNUSED(repository)
}

SkeletonResource::~SkeletonResource() = default;

void SkeletonResource::setSelf(std::shared_ptr<ICourse> self)
{
    d->m_self = self;
}

std::shared_ptr<IEditableCourse> SkeletonResource::self() const
{
    return std::static_pointer_cast<IEditableCourse>(d->m_self.lock());
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
    Q_UNUSED(id)
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
    Q_UNUSED(title)
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
    QFileInfo info(filePath.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash).path());
    if (!info.exists()) {
        qCDebug(ARTIKULATE_LOG()) << "create xml output file directory, not existing";
        QDir dir;
        dir.mkpath(filePath.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash).path());
    }

    // TODO port to atomic file swap
    QFile file(filePath.toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG()) << "Unable to open file " << filePath << " in write mode, aborting.";
        return false;
    }
    file.write(d->serializedSkeleton().toByteArray());

    return true;
}

bool SkeletonResource::createPhraseAfter(IPhrase *previousPhrase)
{
    std::shared_ptr<Unit> parentUnit = units().last();
    if (previousPhrase) {
        for (const auto &unit : units()) {
            if (previousPhrase->unit()->id() == unit->id()) {
                parentUnit = unit;
                break;
            }
        }
    }

    // find index
    int index = parentUnit->phrases().size();
    for (int i = 0; i < parentUnit->phrases().size(); ++i) {
        if (parentUnit->phrases().at(i)->id() == previousPhrase->id()) {
            index = i;
            break;
        }
    }

    // find globally unique phrase id inside course
    QStringList phraseIds;
    for (auto unit : units()) {
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
    phrase->setText(QString());
    phrase->setType(IPhrase::Type::Word);
    parentUnit->addPhrase(phrase, index);

    return true;
}

bool SkeletonResource::deletePhrase(IPhrase *phrase)
{
    Q_ASSERT(phrase);
    if (!phrase) {
        return false;
    }
    auto unitId = phrase->unit()->id();
    for (auto &unit : d->units()) {
        if (unit->id() == unitId) {
            unit->removePhrase(phrase->self());
            return true;
        }
    }
    return false;
}

std::shared_ptr<Unit> SkeletonResource::addUnit(std::shared_ptr<Unit> unit)
{
    std::shared_ptr<Unit> storedUnit(std::move(unit));
    emit unitAboutToBeAdded(storedUnit, d->units().count() - 1);
    d->appendUnit(storedUnit);
    emit unitAdded();
    return storedUnit;
}

bool SkeletonResource::sync()
{
    if (!d->m_modified) {
        qCDebug(ARTIKULATE_LOG()) << "Aborting sync, skeleton was not modified.";
        return false;
    }
    bool ok = exportToFile(file());
    if (ok) {
        d->m_modified = false;
    }
    return ok;
}

void SkeletonResource::updateFrom(std::shared_ptr<ICourse>)
{
    // not supported
}

bool SkeletonResource::isModified() const
{
    return d->m_modified;
}

std::shared_ptr<ILanguage> SkeletonResource::language() const
{
    // skeleton must not have a dedicated language
    return std::shared_ptr<Language>();
}

QString SkeletonResource::languageTitle() const
{
    return QString();
}

void SkeletonResource::setLanguage(std::shared_ptr<ILanguage> language)
{
    Q_UNUSED(language)
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
