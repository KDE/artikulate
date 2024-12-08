/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef EDITORSESSION_H
#define EDITORSESSION_H

#include "core/ieditablerepository.h"
#include "core/ieditableunit.h"
#include "core/iphrase.h"
#include "core/unit.h"
#include "isessionactions.h"
#include <core/ieditablecourse.h>
#include <core/ilanguage.h>
#include <core/iphrase.h>
#include <memory.h>

class ILanguage;
class IEditableCourse;
class IEditableUnit;
class Unit;
class IPhrase;
class SkeletonResource;
class IEditableRepository;

/**
 * \class EditorSession
 */
class EditorSession : public ISessionActions
{
    Q_OBJECT
    Q_INTERFACES(ISessionActions)
    Q_PROPERTY(bool skeletonMode READ skeletonMode NOTIFY skeletonModeChanged)
    /**
     * @brief language of the currently selected course or null if skeleton is selected
     */
    Q_PROPERTY(ILanguage *language READ language NOTIFY languageChanged)
    Q_PROPERTY(IEditableCourse *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(IUnit *unit READ activeUnit NOTIFY unitChanged)
    Q_PROPERTY(IPhrase *phrase READ activePhrase WRITE setActivePhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasNextPhrase READ hasNextPhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasPreviousPhrase READ hasPreviousPhrase NOTIFY phraseChanged)
    Q_PROPERTY(IEditableRepository *repository READ repository WRITE setRepository NOTIFY repositoryChanged)

    QML_ELEMENT
    QML_SINGLETON

public:
    explicit EditorSession(QObject *parent = nullptr);

    IEditableRepository *repository() const;
    void setRepository(IEditableRepository *repository);
    bool skeletonMode() const;
    ILanguage *language() const;
    IEditableCourse *course() const;
    void setCourse(IEditableCourse *course);
    IUnit *activeUnit() const;
    void setActiveUnit(IUnit *unit);
    IPhrase *activePhrase() const;
    void setActivePhrase(IPhrase *phrase) override;
    bool hasPreviousPhrase() const;
    bool hasNextPhrase() const;
    Q_INVOKABLE void switchToPreviousPhrase();
    Q_INVOKABLE void switchToNextPhrase();
    Q_INVOKABLE void updateCourseFromSkeleton();
    TrainingAction *activeAction() const override;
    QVector<TrainingAction *> trainingActions() const override;

Q_SIGNALS:
    void skeletonModeChanged();
    void languageChanged();
    void unitChanged();
    void repositoryChanged();

private:
    Q_DISABLE_COPY(EditorSession)
    void updateTrainingActions();
    void updateActions(std::shared_ptr<IEditableUnit> unit);
    IEditableRepository *m_repository{nullptr};
    bool m_editSkeleton{false};
    IEditableCourse *m_course{nullptr};
    QVector<TrainingAction *> m_actions;
    int m_indexUnit{-1};
    int m_indexPhrase{-1};
};

#endif
