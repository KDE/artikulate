/*
 *  SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef PHRASELISTMODEL_H
#define PHRASELISTMODEL_H

#include "core/phrase.h"
#include <QAbstractListModel>

class Unit;
class QSignalMapper;

class PhraseListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Unit *unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum phraseRoles { TextRole = Qt::UserRole + 1, IdRole, TypeRole, SoundFileRole, ExcludedRole, DataRole };

    explicit PhraseListModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    void setUnit(Unit *unit);
    Unit *unit() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Count phrases in current model view. If this number is changed, signal
     * countChanged() is emitted.
     *
     * \return number of phrases
     */
    int count() const;

Q_SIGNALS:
    void phraseChanged(int index);
    void unitChanged();
    void typeChanged();
    void countChanged();

private Q_SLOTS:
    void onPhraseAboutToBeAdded(std::shared_ptr<IPhrase> unit, int index);
    void onPhraseAdded();
    void onPhraseAboutToBeRemoved(int index);
    void onPhrasesRemoved();
    void emitPhraseChanged(int row);

private:
    void updateMappings();
    Unit *m_unit;
    QSignalMapper *m_signalMapper;
};

#endif
