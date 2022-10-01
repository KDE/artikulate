/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PHONEMEGROUP_H
#define PHONEMEGROUP_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QVector>
#include <memory>

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

private:
    Q_DISABLE_COPY(PhonemeGroup)
    QString m_id;
    QString m_title;
    QString m_description;
    QVector<std::shared_ptr<Phoneme>> m_phonemes;
};

#endif // PHONEMEGROUP_H
