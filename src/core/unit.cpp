/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Oindrila Gupta <oindrila.gupta92@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "unit.h"
#include "phrase.h"

#include <QMap>
#include <QQmlEngine>
#include <QStringList>
#include <QUuid>

#include "artikulate_debug.h"
#include <KLocalizedString>
#include <QUrl>

Unit::Unit(QObject *parent)
    : IEditableUnit(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Unit::~Unit() = default;

std::shared_ptr<Unit> Unit::create()
{
    std::shared_ptr<Unit> unit(new Unit);
    unit->setSelf(unit);
    std::weak_ptr<Unit> unitParameter = unit;
    connect(unit.get(), &IUnit::phraseAdded, unit.get(), [unitParameter]() {
        if (auto unit = unitParameter.lock()) {
            unit->emitPhrasesChanged(unit);
        }
    });
    connect(unit.get(), &IUnit::phraseRemoved, unit.get(), [unitParameter]() {
        if (auto unit = unitParameter.lock()) {
            unit->emitPhrasesChanged(unit);
        }
    });
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

void Unit::addPhrase(std::shared_ptr<IEditablePhrase> phrase, int index)
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
    emit phraseAboutToBeAdded(phrase, index);
    m_phrases.insert(index, phrase);
    emit phraseAdded(phrase);

    connect(phrase.get(), &Phrase::modified, this, &Unit::modified);
    emit modified();
}

void Unit::removePhrase(std::shared_ptr<IPhrase> phrase)
{
    int index = -1;
    for (int i = 0; i < m_phrases.count(); ++i) {
        if (m_phrases.at(i)->id() == phrase->id()) {
            index = i;
            break;
        }
    }
    Q_ASSERT(index >= 0);
    emit phraseAboutToBeRemoved(index);
    m_phrases.removeAt(index);
    emit phraseRemoved();
}

void Unit::emitPhrasesChanged(std::shared_ptr<IEditableUnit> unit)
{
    emit phrasesChanged(unit);
}
