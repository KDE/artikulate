/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PHONEMEMODEL_H
#define PHONEMEMODEL_H

#include <QAbstractListModel>

class Phoneme;
class Language;
class QSignalMapper;

class PhonemeModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Language *language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    enum courseRoles { TitleRole = Qt::UserRole + 1, IdRole, DataRole };

    explicit PhonemeModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    void setLanguage(Language *language);
    Language *language() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void phonemeChanged(int index);
    void languageChanged();

private Q_SLOTS:
    void onPhonemeAboutToBeAdded(Phoneme *phoneme, int index);
    void onPhonemeAdded();
    void onPhonemesAboutToBeRemoved(int first, int last);
    void onPhonemesRemoved();
    void emitPhonemeChanged(int row);

private:
    void updateMappings();
    Language *m_language;
    QSignalMapper *m_signalMapper;
};

#endif // PHONEMEMODEL_H
