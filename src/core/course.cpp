/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "course.h"
#include "unit.h"
#include "language.h"
#include "resources/resourceinterface.h"
#include "resources/courseresource.h"
#include "resourcemanager.h"
#include "phonemegroup.h"

#include "artikulate_debug.h"
#include <KLocalizedString>
#include <QStringList>
#include <QPair>
#include <QUuid>

Course::Course(ResourceInterface *resource)
    : QObject(resource)
    , m_resource(qobject_cast<CourseResource*>(resource))
    , m_language(nullptr)
    , m_modified(false)
{
}

Course::~Course()
{
    foreach (Unit *unit, m_unitList) {
        unit->deleteLater();
    }
    m_unitList.clear();

    // clear phonom units
    QMultiMap< PhonemeGroup*, QList< QPair<Phoneme*, Unit*> > >::iterator groupIter = m_phonemeUnitList.begin();
    while (groupIter != m_phonemeUnitList.end()) {
        QList< QPair<Phoneme*, Unit*> >::iterator itemIter = groupIter->begin();
        while (itemIter != groupIter->end()) {
            itemIter->first->deleteLater();     // delete phoneme
            itemIter->second->deleteLater();    // delete unit
            ++itemIter;
        }
        groupIter->clear();
        ++groupIter;
    }
    m_phonemeUnitList.clear();
    m_phonemeGroupList.clear();
}

QString Course::id() const
{
    return m_id;
}

void Course::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
        setModified();
    }
}

QString Course::foreignId() const
{
    return m_foreignId;
}

void Course::setForeignId(const QString &id)
{
    m_foreignId = id;
}

QString Course::title() const
{
    return m_title;
}

QString Course::i18nTitle() const
{
    return m_resource->i18nTitle();
}

void Course::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
        setModified();
    }
}

QString Course::description() const
{
    return m_description;
}

void Course::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

Language * Course::language() const
{
    return m_language;
}

void Course::setLanguage(Language *language)
{
    Q_ASSERT(language);
    // TODO this should happen in the ctor
    foreach (PhonemeGroup *group, language->phonemeGroups()) {
        addPhonemeGroup(group);
    }
    m_language = language;
    emit languageChanged();
}

QUrl Course::file() const
{
    return m_file;
}

void Course::setFile(const QUrl &file)
{
    m_file = file;
}

QList< Unit* > Course::unitList() const
{
    return m_unitList;
}

void Course::addUnit(Unit *unit)
{
    QList<Unit*>::ConstIterator iter = m_unitList.constBegin();
    while (iter != m_unitList.constEnd()) {
        if (unit->id() == (*iter)->id()) {
            qCWarning(ARTIKULATE_LOG) << "Unit already contained in this course, aborting";
            return;
        }
        ++iter;
    }
    emit unitAboutToBeAdded(unit, m_unitList.length());
    m_unitList.append(unit);

    connect(unit, &Unit::modified, this, [=]() {
        setModified(true);
    });

    // these connections are only present for "normal units" and take care to register
    // there phrases also at phoneme units
    // TODO: removing of phrase and upading of phonemes for that case is not implemented
    connect(unit, &Unit::phraseAdded, this, &Course::registerPhrasePhonemes);

    emit unitAdded();
    setModified(true);
}

Unit * Course::createUnit()
{
    // find first unused id
    QStringList unitIds;
    foreach (Unit *unit, m_unitList) {
        unitIds.append(unit->id());
    }
    QString id = QUuid::createUuid().toString();
    while (unitIds.contains(id)) {
        id = QUuid::createUuid().toString();
        qCWarning(ARTIKULATE_LOG) << "Unit id generator has found a collision, recreating id.";
    }

    // create unit
    Unit *unit = new Unit(this);
    unit->setCourse(this);
    unit->setId(id);
    unit->setTitle(i18n("New Unit"));
    addUnit(unit);

    return unit;
}

Phrase * Course::createPhrase(Unit *unit)
{
    // find globally unique phrase id inside course
    QStringList phraseIds;
    foreach (Unit *unit, m_unitList) {
        foreach (Phrase *phrase, unit->phraseList()) {
            phraseIds.append(phrase->id());
        }
    }
    QString id = QUuid::createUuid().toString();
    while (phraseIds.contains(id)) {
        id = QUuid::createUuid().toString();
        qCWarning(ARTIKULATE_LOG) << "Phrase id generator has found a collision, recreating id.";
    }

    // create unit
    Phrase *phrase = new Phrase(this);
    phrase->setId(id);
    phrase->setText(QLatin1String(""));
    phrase->setType(Phrase::Word);

    unit->addPhrase(phrase);

    return phrase;
}

QList< Unit* > Course::phonemeUnitList(PhonemeGroup *phonemeGroup) const
{
    QList<Unit*> list;
    for (const auto &group : m_phonemeUnitList.value(phonemeGroup)) {
        list.append(group.second);
    }
    return list;
}

Unit * Course::phonemeUnit(Phoneme *phoneme) const
{
    for (auto group = m_phonemeUnitList.keyBegin(); group != m_phonemeUnitList.keyEnd(); ++group) {
        m_phonemeUnitList.value(*group);
        for (const auto &phonemeUnit : m_phonemeUnitList.value(*group)) {
            if (phonemeUnit.first == phoneme) {
                return phonemeUnit.second;
            }
        }
    }
    return nullptr;
}

PhonemeGroup * Course::phonemeGroup(Unit *unit) const
{
    for (auto group = m_phonemeUnitList.keyBegin(); group != m_phonemeUnitList.keyEnd(); ++group) {
        m_phonemeUnitList.value(*group);
        for (const auto &phonemeUnit : m_phonemeUnitList.value(*group)) {
            if (phonemeUnit.second == unit) {
                return *group;
            }
        }
    }
    return nullptr;
}

void Course::addPhonemeGroup(PhonemeGroup *phonemeGroup)
{
    if (m_phonemeUnitList.contains(phonemeGroup)) {
        qCWarning(ARTIKULATE_LOG) << "Phoneme group already contained in this course, aborting";
        return;
    }
    emit phonemeGroupAboutToBeAdded(phonemeGroup, m_phonemeGroupList.count());

    // add to phoneme list
    m_phonemeGroupList.append(phonemeGroup);
    m_phonemeUnitList.insert(phonemeGroup, QList< QPair<Phoneme *, Unit *> >());

    emit phonemeGroupAdded();
    setModified();
}

QList<PhonemeGroup *> Course::phonemeGroupList() const
{
    return m_phonemeGroupList;
}

bool Course::modified() const
{
    return m_modified;
}

void Course::setModified(bool modified)
{
    if (m_modified == modified) {
        return;
    }
    m_modified = modified;
    emit modifiedChanged();
}

void Course::sync()
{
    if (!m_file.isValid() || m_file.isEmpty() || m_resource == nullptr) {
        qCritical() << "Path" << m_file.toLocalFile() << "not valid, aborting sync operation.";
        return;
    }
    m_resource->sync();
    setModified(false);
}

bool Course::isContributorResource() const
{
    return m_resource->isContributorResource();
}

void Course::registerPhrasePhonemes(Phrase *phrase)
{
    // iterate over all phonemes of this phrase
    foreach (Phoneme *phoneme, phrase->phonemes()) {
        // try to find corresponding phonem groups (phonem groups are registered on course creation)
        foreach (PhonemeGroup *group, m_phonemeGroupList) {
            if (!group->contains(phoneme)) {
                continue;
            }
            // either add phrase to existing unit or register a new one
            bool phraseRegistered = false;
            for (const auto &phonemeUnit : m_phonemeUnitList.value(group)) {
                if (phonemeUnit.first->id() == phoneme->id()) {
                    phonemeUnit.second->addPhrase(phrase);
                    phraseRegistered = true;
                }
            }
            // otherwise, need to create a new unit
            if (phraseRegistered == false) {
                // create unit based on the phoneme group
                Unit *unit = new Unit(this);
                unit->setId(phoneme->id());
                unit->setTitle(phoneme->title());
                unit->setCourse(this);
                m_phonemeUnitList[group].append(qMakePair<Phoneme*,Unit*>(phoneme, unit));
                unit->addPhrase(phrase);
            }
        }
    }
}
