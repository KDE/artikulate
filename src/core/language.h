/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "artikulatecore_export.h"
#include "ilanguage.h"
#include <QObject>
#include <QUrl>
#include <QVector>
#include <memory>

class QString;
class Phoneme;
class PhonemeGroup;

class ARTIKULATECORE_EXPORT Language : public ILanguage
{
    Q_OBJECT
    Q_INTERFACES(ILanguage)

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString i18nTitle READ i18nTitle WRITE seti18nTitle NOTIFY i18nTitleChanged)

public:
    static std::shared_ptr<Language> create(QUrl file);
    ~Language() override;

    QString id() const override;
    void setId(const QString &id);
    QString title() const override;
    void seti18nTitle(const QString &title);
    QString i18nTitle() const override;
    void setTitle(const QString &title);
    QUrl file() const;
    void setFile(const QUrl &file);
    QVector<std::shared_ptr<Phoneme>> phonemes() const override;
    QVector<std::shared_ptr<PhonemeGroup>> phonemeGroups() const override;

Q_SIGNALS:
    void idChanged();
    void titleChanged();
    void i18nTitleChanged();
    void phonemesChanged();

protected:
    explicit Language();

private:
    QString m_id;
    QString m_title;
    QString m_i18nTitle;
    QUrl m_file;
    QVector<std::shared_ptr<PhonemeGroup>> m_phonemeGroups;
};

#endif // LANGUAGE_H
