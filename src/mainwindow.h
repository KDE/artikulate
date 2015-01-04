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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QQuickWidget>
#include "core/resourcemanager.h"
#include "core/trainingsession.h"

class TrainingSession2;
class EditorSession;
class KActionCollection;
class KMenu;
class QQuickWidget;

namespace LearnerProfile {
    class ProfileManager;
}

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
    Q_PROPERTY(ResourceManager *globalResourceManager READ resourceManager CONSTANT)

public:
    Q_ENUMS(ViewMode)
    enum ViewMode {
        Trainer,
        Editor
    };

    /**
     * Default Constructor
     */
    MainWindow();

    /**
     * Default Destructor
     */
    virtual ~MainWindow();

    ResourceManager * resourceManager() const;

    void setupActions();

    virtual QSize sizeHint() const { return QSize(1000, 700); }

    bool queryClose();

public Q_SLOTS:
    void showCourseEditor();
    void closeCourseEditor();
    void switchMode();
    void showSettingsDialog();
    void downloadNewStuff();
    void updateTrainingPhraseFont();
    void updateKcfgUseContributorResources();
    void configLearnerProfile();

Q_SIGNALS:
     void modeChanged(bool);

private:
    TrainingSession2 *m_trainingSession2;
    EditorSession *m_editorSession;
    ResourceManager *m_resourceManager;
    TrainingSession *m_trainingSession;
    LearnerProfile::ProfileManager *m_profileManager;
    QQuickWidget *m_widget;
};

#endif // PAIRS_H
