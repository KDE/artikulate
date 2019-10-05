/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef EDITORSESSION_H
#define EDITORSESSION_H

#include "artikulatecore_export.h"
#include "phrase.h"
#include "isessionactions.h"

class ILanguage;
class IEditableCourse;
class Unit;
class IPhrase;
class SkeletonResource;
class IEditableRepository;

/**
 * \class EditorSession
 */
class ARTIKULATECORE_EXPORT EditorSession : public ISessionActions
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

public:
    explicit EditorSession(QObject *parent = nullptr);

    void setRepository(IEditableRepository *repository);
    bool skeletonMode() const;
    ILanguage * language() const;
    IEditableCourse * course() const;
    void setCourse(IEditableCourse *course);
    IUnit * activeUnit() const;
    void setActiveUnit(IUnit *unit);
    IPhrase * activePhrase() const;
    void setActivePhrase(IPhrase *phrase) override;
    bool hasPreviousPhrase() const;
    bool hasNextPhrase() const;
    Q_INVOKABLE void switchToPreviousPhrase();
    Q_INVOKABLE void switchToNextPhrase();
    Q_INVOKABLE void updateCourseFromSkeleton();
    TrainingAction * activeAction() const override;
    QVector<TrainingAction *> trainingActions() const override;

Q_SIGNALS:
    void skeletonModeChanged();
    void languageChanged();
    void unitChanged();

private:
    Q_DISABLE_COPY(EditorSession)
    void updateTrainingActions();
    IEditableRepository * m_repository{ nullptr };
    bool m_editSkeleton{ false };
    IEditableCourse *m_course{ nullptr };
    QVector<TrainingAction*> m_actions;
    int m_indexUnit{-1};
    int m_indexPhrase{-1};
};

#endif
