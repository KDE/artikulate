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

#include "unit.h"
#include "phrase.h"

#include <QMap>
#include <QList>
#include <QSignalMapper>
#include <QStringList>
#include <QUuid>
#include <QQmlEngine>

#include "artikulate_debug.h"
#include <KLocalizedString>
#include <QUrl>

Unit::Unit(QObject *parent)
    : IEditableUnit(parent)
    , m_phraseSignalMapper(new QSignalMapper(this))
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Unit::~Unit()
{
    for (auto phrase : m_phrases) {
        phrase->deleteLater();
    }
    m_phrases.clear();
    m_phraseSignalMapper->deleteLater();
}

std::shared_ptr<Unit> Unit::create()
{
    std::shared_ptr<Unit> unit(new Unit);
    unit->setSelf(unit);
    return unit;
}

void Unit::setSelf(std::shared_ptr<IUnit> self)
{
    m_self = self;
}

std::shared_ptr<IUnit> Unit::self() const
{
     return m_self.lock();
}

QString Unit::id() const
{
    return m_id;
}

void Unit::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
        emit modified();
    }
}

QString Unit::foreignId() const
{
    return m_foreignId;
}

void Unit::setForeignId(const QString &id)
{
    m_foreignId = id;
}

std::shared_ptr<ICourse> Unit::course() const
{
    return m_course.lock();
}

void Unit::setCourse(std::shared_ptr<ICourse> course)
{
    if (course == m_course.lock()) {
        return;
    }
    m_course = course;
    emit courseChanged();
}

QString Unit::title() const
{
    return m_title;
}

void Unit::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
        emit modified();
    }
}

QVector<std::shared_ptr<IPhrase>> Unit::phrases() const
{
    return m_phrases;
}

void Unit::addPhrase(std::shared_ptr<IEditablePhrase> phrase)
{
    auto iter = m_phrases.constBegin();
    while (iter != m_phrases.constEnd()) {
        if (phrase->id() == (*iter)->id()) {
            qCWarning(ARTIKULATE_LOG()) << "Phrase is already contained in this unit, aborting";
            return;
        }
        ++iter;
    }
    phrase->setUnit(m_self.lock());
    emit phraseAboutToBeAdded(phrase, m_phrases.length());
    m_phrases.append(phrase);
    m_phraseSignalMapper->setMapping(phrase.get(), phrase->id());

    emit phraseAdded(phrase);

    connect(phrase.get(), &Phrase::typeChanged, m_phraseSignalMapper,
        static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(phrase.get(), &Phrase::modified, this, &Unit::modified);

    emit modified();
}

QList<IPhrase *> Unit::excludedSkeletonPhraseList() const
{
    QList<IPhrase *> excludedPhraseList;
//TODO this should not be handled on unit level
//    for (auto phrase : m_phrases) {
//        if (phrase->isExcluded() == true) {
//            excludedPhraseList.append(phrase);
//        }
//    }
    return excludedPhraseList;
}

void Unit::excludeSkeletonPhrase(const QString &phraseId)
{
//    for (auto phrase : m_phrases) {
//        if (phrase->id() == phraseId) {
//            phrase->setExcluded(true);
//            emit modified();
//            return;
//        }
//    }
    qCWarning(ARTIKULATE_LOG) << "Could not exclude phrase with ID " << phraseId << ", no phrase with this ID.";
}

void Unit::includeSkeletonPhrase(const QString &phraseId)
{
//    for (auto phrase : m_phrases) {
//        if (phrase->id() == phraseId) {
//            phrase->setExcluded(false);
//            emit modified();
//            return;
//        }
//    }
    qCWarning(ARTIKULATE_LOG) << "Could not include phrase with ID " << phraseId << ", no phrase with this ID.";
}
