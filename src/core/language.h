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

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "artikulatecore_export.h"
#include <memory>
#include <QObject>
#include <QVector>
#include <QUrl>

class QString;
class Phoneme;
class PhonemeGroup;

class ARTIKULATECORE_EXPORT Language : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString i18nTitle READ i18nTitle WRITE seti18nTitle NOTIFY i18nTitleChanged)

public:
    explicit Language();
    ~Language();

    QString id() const;
    void setId(const QString &id);
    QString title() const;
    void seti18nTitle(const QString &title);
    QString i18nTitle() const;
    void setTitle(const QString &title);
    QUrl file() const;
    void setFile(const QUrl &file);
    QVector<std::shared_ptr<Phoneme>> phonemes() const;
    QVector<std::shared_ptr<PhonemeGroup>> phonemeGroups() const;
    std::shared_ptr<PhonemeGroup> addPhonemeGroup(const QString &identifier, const QString &title);

Q_SIGNALS:
    void idChanged();
    void associatedLanguageItemChanged();
    void titleChanged();
    void i18nTitleChanged();
    void phonemesChanged();
    void phonemeGroupsChanged();

private:
    QString m_id;
    QString m_title;
    QString m_i18nTitle;
    QUrl m_file;
    QVector<std::shared_ptr<PhonemeGroup>> m_phonemeGroups;
};

#endif // LANGUAGE_H
