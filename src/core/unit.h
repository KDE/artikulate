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
#include "ieditableunit.h"
#include <QObject>
#include <QStringList>
#include <QVector>
#include <memory>

class QString;
class Phrase;
class IPhrase;
class ICourse;

class ARTIKULATECORE_EXPORT Unit : public IEditableUnit
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    static std::shared_ptr<Unit> create();
    ~Unit() override;

    QString id() const override;
    void setId(const QString &id) override;
    QString foreignId() const override;
    void setForeignId(const QString &id) override;
    std::shared_ptr<ICourse> course() const override;
    void setCourse(std::shared_ptr<ICourse> course) override;
    QString title() const override;
    void setTitle(const QString &title) override;
    QVector<std::shared_ptr<IPhrase>> phrases() const override;
    void addPhrase(std::shared_ptr<IEditablePhrase> phrase) override;
    QList<IPhrase *> excludedSkeletonPhraseList() const;
    std::shared_ptr<IUnit> self() const override;

    /**
     * Removes phrase with ID \p phraseId from unit and adds ID to set
     * of excluded IDs.
     *
     * \param phraseId is the UID of the to be excluded phrase
     */
    Q_INVOKABLE void excludeSkeletonPhrase(const QString &phraseId);
    Q_INVOKABLE void includeSkeletonPhrase(const QString &phraseId);

protected:
    explicit Unit(QObject *parent = nullptr);

private:
    void setSelf(std::shared_ptr<IUnit> self) override;
    Q_DISABLE_COPY(Unit)
    std::weak_ptr<IUnit> m_self;
    QString m_id;
    QString m_foreignId;
    std::weak_ptr<ICourse> m_course;
    QString m_title;
    QVector<std::shared_ptr<IPhrase>> m_phrases;
};

#endif // UNIT_H
