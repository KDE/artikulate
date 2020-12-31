/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PHRASEMODEL_H
#define PHRASEMODEL_H

#include "core/phrase.h"
#include <QAbstractItemModel>
#include <memory>

class ICourse;
class QSignalMapper;

class PhraseModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)

public:
    enum phraseRoles { TextRole = Qt::UserRole + 1, IdRole, DataRole };

    explicit PhraseModel(QObject *parent = nullptr);
    virtual QHash<int, QByteArray> roleNames() const override;
    void setCourse(ICourse *course);
    ICourse *course() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool isPhrase(const QModelIndex &index) const;
    Q_INVOKABLE IPhrase *phrase(const QModelIndex &index) const;
    Q_INVOKABLE Unit *unit(const QModelIndex &index) const;
    Q_INVOKABLE QModelIndex indexPhrase(Phrase *phrase) const;
    Q_INVOKABLE QModelIndex indexUnit(Unit *unit) const;
    Q_INVOKABLE bool isUnit(const QModelIndex &index) const;

Q_SIGNALS:
    void phraseChanged(int index);
    void courseChanged();
    void typeChanged();

private Q_SLOTS:
    void onPhraseAboutToBeAdded(std::shared_ptr<IPhrase> phrase, int index);
    void onPhraseAdded();
    void onPhraseAboutToBeRemoved(int index);
    void onPhrasesRemoved();
    void onPhraseChanged(QObject *phrase);
    void onUnitAboutToBeAdded(std::shared_ptr<Unit> unit, int index);
    void onUnitAdded();
    void onUnitsAboutToBeRemoved(int first, int last);
    void onUnitsRemoved();
    void onUnitChanged(int index);

private:
    void updateUnitMappings();
    void updatePhraseMappings();

    ICourse *m_course;
    QSignalMapper *m_unitSignalMapper;
    QSignalMapper *m_phraseSignalMapper;
};

#endif
