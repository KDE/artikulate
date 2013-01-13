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

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QObject>
#include <QList>
#include <KUrl>

class QString;
class Tag;
class TagGroup;

class Language : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    explicit Language(QObject *parent = 0);
    ~Language();

    QString id() const;
    void setId(const QString &id);
    QString title() const;
    void setTitle(const QString &title);
    KUrl file() const;
    void setFile(const KUrl &file);
    QList<Tag *> prononciationTags() const;
    Tag * addPrononciationTag(const QString &identifier, const QString &title);
    QList<TagGroup *> prononciationGroups() const;
    TagGroup * addPrononciationGroup(const QString &identifier, const QString &title);

signals:
    void idChanged();
    void titleChanged();
    void tagsChanged();
    void groupsChanged();

private:
    Q_DISABLE_COPY(Language)
    QString m_id;
    QString m_title;
    KUrl m_file;
    QList<Tag *> m_prononciationTags;
    QList<TagGroup *> m_prononciationGroups;
};

#endif // LANGUAGE_H
