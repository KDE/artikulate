/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core/resourcerepository.h"
#include <KActionCollection>
#include <QQmlApplicationEngine>

class TrainingSession;
class KHelpMenu;

namespace LearnerProfile
{
class ProfileManager;
}

class MainWindow : public QQmlApplicationEngine
{
    Q_OBJECT

public:
    /**
     * Default Constructor
     */
    MainWindow();

    /**
     * Default Destructor
     */
    ~MainWindow() override;

    const IResourceRepository *resourceRepository() const;

    KActionCollection *actionCollection();
    void setupActions();
    bool queryClose();

public Q_SLOTS:
    void showSettingsDialog();
    void updateCourseResources();
    void updateTrainingPhraseFont();
    void updateKcfgUseContributorResources();
    void configLearnerProfile();
    void switchMenuBarVisibility();
    void triggerAction(const QString &);

private:
    KActionCollection *m_actionCollection;
    KHelpMenu *m_helpMenu;
    LearnerProfile::ProfileManager *m_profileManager;
    TrainingSession *m_trainingSession;
};

#endif
