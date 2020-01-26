/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
    void addPhrase(std::shared_ptr<IEditablePhrase> phrase, int index) override;
    void removePhrase(std::shared_ptr<IPhrase> phrase) override;
    std::shared_ptr<IUnit> self() const override;
    void emitPhrasesChanged(std::shared_ptr<IEditableUnit> unit);

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
