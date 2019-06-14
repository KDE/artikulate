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

#ifndef PHONEMEGROUP_H
#define PHONEMEGROUP_H

#include "artikulatecore_export.h"
#include <memory>
#include <QVector>
#include <QObject>
#include <QMap>

class QString;
class Phoneme;

/**
 * \class PhonemeGroup
 */
class ARTIKULATECORE_EXPORT PhonemeGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    explicit PhonemeGroup();
    ~PhonemeGroup() override;
    QString id() const;
    void setId(const QString &id);
    QString title() const;
    void setTitle(const QString &title);
    QString description() const;
    void setDescription(const QString &description);
    std::shared_ptr<Phoneme> addPhoneme(std::unique_ptr<Phoneme> phoneme);
    std::shared_ptr<Phoneme> addPhoneme(const QString &identifier, const QString &title);
    QVector<std::shared_ptr<Phoneme>> phonemes() const;
    /**
     * Checks by identifier comparison whether phoneme is registered in this group.
     *
     * \param poneme is the phoneme to be checked for if registered
     * \return true if registered, false otherwise
     */
    bool contains(std::shared_ptr<Phoneme> phoneme) const;

Q_SIGNALS:
    void idChanged();
    void titleChanged();
    void descriptionChanged();
    void phonemeAdded(const Phoneme&);

private:
    Q_DISABLE_COPY(PhonemeGroup)
    QString m_id;
    QString m_title;
    QString m_description;
    QVector<std::shared_ptr<Phoneme>> m_phonemes;
};

#endif // PHONEMEGROUP_H
