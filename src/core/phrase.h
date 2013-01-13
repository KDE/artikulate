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

#ifndef PHRASE_H
#define PHRASE_H

#include <QObject>
#include <KUrl>
#include <QList>

class QString;
class Tag;
class KUrl;

class Phrase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(KUrl sound READ sound WRITE setSound NOTIFY soundChanged)

    typedef enum {
        Word,
        Expression,
        Sentence,
        Paragraph
    } Type;

public:
    explicit Phrase(QObject *parent = 0);

    QString id() const;
    void setId(const QString &id);
    QString text() const;
    void setText(const QString &title);
    Phrase::Type type() const;
    QString typeString() const;
    void setType(Phrase::Type type);
    void setType(const QString &typeString);
    KUrl sound() const;
    void setSound(const KUrl &soundFile);
    QList<Tag *> prononciationTags() const;
    void addPrononciationTag(Tag *tag);

signals:
    void idChanged();
    void unitChanged();
    void textChanged();
    void soundChanged();
    void prononciationTagsChanged();

private:
    Q_DISABLE_COPY(Phrase)
    QString m_id;
    QString m_text;
    Type m_type;
    KUrl m_sound;
    QList<Tag *> m_prononciationTags;
};

#endif // LESSON_H