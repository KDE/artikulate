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

#ifndef UNIT_H
#define UNIT_H

#include "artikulatecore_export.h"
#include <memory>
#include <QObject>
#include <QVector>
#include <QStringList>

class QSignalMapper;
class QString;
class Phrase;
class IPhrase;
class ICourse;

class ARTIKULATECORE_EXPORT Unit : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)

public:
    explicit Unit(QObject *parent = nullptr);
    ~Unit();

    QString id() const;
    void setId(const QString &id);
    QString foreignId() const;
    void setForeignId(const QString &id);
    ICourse * course() const;
    void setCourse(ICourse* course);
    QString title() const;
    void setTitle(const QString &title);
    QVector<std::shared_ptr<IPhrase>> phrases() const;
    void addPhrase(std::shared_ptr<Phrase> phrase);
    QList<IPhrase *> excludedSkeletonPhraseList() const;

    /**
     * Removes phrase with ID \p phraseId from unit and adds ID to set
     * of excluded IDs.
     *
     * \param phraseId is the UID of the to be excluded phrase
     */
    Q_INVOKABLE void excludeSkeletonPhrase(const QString &phraseId);
    Q_INVOKABLE void includeSkeletonPhrase(const QString &phraseId);

Q_SIGNALS:
    void idChanged();
    void titleChanged();
    void courseChanged();
    void displayPhraseTypeChanged();
    void modified();
    void phraseAdded(IPhrase*); //TODO
    void phraseAboutToBeAdded(IPhrase*,int);//TODO
    void phraseRemoved(IPhrase*);//TODO
    void phraseAboutToBeRemoved(int,int);

private:
    Q_DISABLE_COPY(Unit)
    QString m_id;
    QString m_foreignId;
    ICourse *m_course;
    QString m_title;
    QVector<std::shared_ptr<IPhrase>> m_phrases;
    QSignalMapper *m_phraseSignalMapper;
};

#endif // UNIT_H
