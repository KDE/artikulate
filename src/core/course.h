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

#ifndef COURSE_H
#define COURSE_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QMap>
#include <KUrl>

class QString;
class Language;
class Unit;
class Phrase;
class PhonemeGroup;
class Phoneme;

class ARTIKULATELIB_EXPORT Course : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged)
    Q_PROPERTY(Language * language READ language NOTIFY languageChanged)

public:
    explicit Course(QObject *parent = 0);
    ~Course();
    QString id() const;
    void setId(const QString &id);
    QString foreignId() const;
    void setForeignId(const QString &id);
    QString title() const;
    void setTitle(const QString &title);
    Language * language() const;
    void setLanguage(Language *language);
    QString description() const;
    void setDescription(const QString &description);
    KUrl file() const;
    void setFile(const KUrl &file);
    QList<Unit *> unitList() const;
    QList<Unit *> phonemeUnitList(PhonemeGroup *phonemeGroup) const;
    /**
     * \return the corresponding unit for phoneme \p phoneme
     */
    Unit * phonemeUnit(Phoneme *phoneme) const;
    /**
     * \return the phoneme group containing the phoneme corresponding to \p unit
     */
    PhonemeGroup * phonemeGroup(Unit *unit) const;
    void addUnit(Unit *unit);
    QList<PhonemeGroup *> phonemeGroupList() const;
    void addPhonemeGroup(PhonemeGroup *phonemeGroup);

    /**
     * Create and add a new unit to course.
     *
     * \return pointer to the created unit
     */
    Q_INVOKABLE Unit * createUnit();

    /**
     * Create and add a new phrase and add it to the specified unit. The type of the created phrase
     * is initially Phrase::Word.
     *
     * \param unit the unit to that the created hprase shall be added
     * \return pointer to the created phrase
     */
    Q_INVOKABLE Phrase * createPhrase(Unit *unit);

    /**
     * \return true if the course was modified after the last sync, otherwise false
     */
    bool modified() const;

    /**
     * Writes course object back to file and set \ref modified state to false.
     * If no file is set, no operation is performed.
     */
    virtual Q_INVOKABLE void sync();

signals:
    void idChanged();
    void titleChanged();
    void descriptionChanged();
    void modifiedChanged();
    void languageChanged();
    void unitAdded();
    void unitAboutToBeAdded(Unit*,int);
    void unitRemoved();
    void unitAboutToBeRemoved(int,int);
    void phonemeGroupAdded();
    void phonemeGroupAboutToBeAdded(PhonemeGroup*,int);
    void phonemeGroupRemoved();
    void phonemeGroupAboutToBeRemoved(int,int);

private:
    Q_DISABLE_COPY(Course)
    QString m_id;
    QString m_foreignId;
    QString m_title;
    QString m_description;
    Language *m_language;
    KUrl m_file;
    bool m_modified;
    QList<Unit *> m_unitList;
    QList<PhonemeGroup *> m_phonemeGroupList;
    QMap< PhonemeGroup *, QList< QPair<Phoneme *, Unit *> > >m_phonemeUnitList;

public slots:
    void setModified(bool modified = true);
    void registerPhrasePhonemes(Phrase *phrase);
    void removePhrasePhonemes(Phrase *phrase);
};

#endif // COURSE_H
