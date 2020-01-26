/*
 *  Copyright 2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef MAINWINDOW_EDITOR_H
#define MAINWINDOW_EDITOR_H

#include "core/contributorrepository.h"
#include "core/trainingsession.h"
#include <QQmlApplicationEngine>

class EditorSession;
class QQuickWidget;

class MainWindowEditor : public QQmlApplicationEngine
{
    Q_OBJECT

public:
    /**
     * Default Constructor
     */
    explicit MainWindowEditor(ContributorRepository *repository);

    /**
     * Default Destructor
     */
    virtual ~MainWindowEditor();

    ContributorRepository *resourceRepository() const;

    void setupActions();

public Q_SLOTS:
    void showSettingsDialog();
    void save();
    void quit();

Q_SIGNALS:
    void modeChanged(bool);

private:
    ContributorRepository *m_repository;
    EditorSession *m_editorSession;
};

#endif
