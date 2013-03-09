/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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
#include "resourcemanager.h"
#include "phonemegroup.h"

#include <KDebug>
#include <KLocale>
#include <QStringList>
#include <QPair>
#include <QUuid>

Course::Course(QObject *parent)
    : QObject(parent)
    , m_language(0)
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

QString Course::title() const
{
    return m_title;
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
}

KUrl Course::file() const
{
    return m_file;
}

void Course::setFile(const KUrl &file)
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
            kWarning() << "Unit already contained in this course, aborting";
            return;
        }
        ++iter;
    }
    emit unitAboutToBeAdded(unit, m_unitList.length());
    m_unitList.append(unit);

    connect(unit, SIGNAL(modified()), this, SLOT(setModified()));

    // these connections are only present for "normal units" and take care to register
    // there phrases also at phoneme units
    connect(unit, SIGNAL(phraseAdded(Phrase *)), this, SLOT(registerPhrasePhonemes(Phrase*)));
    connect(unit, SIGNAL(phraseRemoved(Phrase *)), this, SLOT(removePhrasePhonemes(Phrase*)));

    emit unitAdded();
    setModified();
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
        id = QUuid::createUuid();
        kWarning() << "Unit id generator has found a collision, recreating id.";
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
        foreach (Phrase *phrase, unit->phraseList(Phrase::AllTypes)) {
            phraseIds.append(phrase->id());
        }
    }
    QString id = QUuid::createUuid().toString();
    while (phraseIds.contains(id)) {
        id = QUuid::createUuid();
        kWarning() << "Phrase id generator has found a collision, recreating id.";
    }

    // create unit
    Phrase *phrase = new Phrase(this);
    phrase->setId(id);
    phrase->setText("");
    phrase->setType(Phrase::Word);

    unit->addPhrase(phrase);

    return phrase;
}

QList< Unit* > Course::phonemeUnitList(PhonemeGroup *phonemeGroup) const
{
    QList<Unit*> list;
    QList< QPair<Phoneme*,Unit*> >::ConstIterator iter = m_phonemeUnitList.value(phonemeGroup).constBegin();
    while (iter != m_phonemeUnitList.value(phonemeGroup).constEnd()) {
        list.append(iter->second);
        ++iter;
    }
    return list;
}

Unit * Course::phonemeUnit(Phoneme *phoneme) const
{
    foreach (PhonemeGroup *group, m_phonemeUnitList.keys()) {
        m_phonemeUnitList.value(group);
        QList< QPair<Phoneme*,Unit*> >::ConstIterator iter = m_phonemeUnitList.value(group).constBegin();
        while (iter != m_phonemeUnitList.value(group).constEnd()) {
            if (iter->first == phoneme) {
                return iter->second;
            }
            ++iter;
        }
    }
    return 0;
}

PhonemeGroup * Course::phonemeGroup(Unit *unit) const
{
    foreach (PhonemeGroup *group, m_phonemeUnitList.keys()) {
        m_phonemeUnitList.value(group);
        QList< QPair<Phoneme*,Unit*> >::ConstIterator iter = m_phonemeUnitList.value(group).constBegin();
        while (iter != m_phonemeUnitList.value(group).constEnd()) {
            if (iter->second == unit) {
                return group;
            }
            ++iter;
        }
    }
    return 0;
}

void Course::addPhonemeGroup(PhonemeGroup *phonemeGroup)
{
    if (m_phonemeUnitList.contains(phonemeGroup)) {
        kWarning() << "Phoneme group already contained in this course, aborting";
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
    if (!m_file.isValid() || m_file.isEmpty()) {
        kWarning() << "No file path set, aborting sync operation.";
        return;
    }

    // call sync operation
    ResourceManager::sync(this);
    setModified(false);
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
            QList< QPair<Phoneme*,Unit*> >::ConstIterator iter = m_phonemeUnitList.value(group).constBegin();
            while (iter != m_phonemeUnitList.value(group).constEnd()) {
                if (iter->first->id() == phoneme->id()) {
                    iter->second->addPhrase(phrase);
                    phraseRegistered = true;
                }
                ++iter;
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

void Course::removePhrasePhonemes(Phrase* phrase)
{
    kError() << "Not yet implemented!";
}
