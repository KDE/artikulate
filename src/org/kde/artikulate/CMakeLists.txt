add_library(artikluate_backing STATIC)

qt_add_qml_module(
    artikluate_backing
    URI "org.kde.artikulate"
    IMPORTS
        org.kde.kirigami
        org.kde.kirigamiaddons.formcard
    SOURCES
        drawertrainingactions.cpp
        drawertrainingactions.h
        editorsession.cpp
        editorsession.h
        isessionactions.h
        player.cpp
        player.h
        recorder.cpp
        recorder.h
        trainingaction.cpp
        trainingaction.h
        trainingactionicon.cpp
        trainingactionicon.h
        trainingsession.cpp
        trainingsession.h
        # models
        coursemodel.cpp
        coursemodel.h
        coursefiltermodel.cpp
        coursefiltermodel.h

        drawercoursetreemodel.cpp
        drawercoursetreemodel.h
        languagemodel.cpp
        languagemodel.h
        languageresourcemodel.cpp
        languageresourcemodel.h
    #     learningprogressmodel.cpp //TODO must be adapted to new trainingsession
        unitmodel.cpp
        unitmodel.h
        unitfiltermodel.cpp
        unitfiltermodel.h
        phrasemodel.cpp
        phrasemodel.h
        phraselistmodel.cpp
        phraselistmodel.h
        phrasefiltermodel.cpp
        phrasefiltermodel.h
        phonememodel.cpp
        phonememodel.h
        phonemegroupmodel.cpp
        phonemegroupmodel.h
        phonemeunitmodel.cpp
        phonemeunitmodel.h
        profilemodel.cpp
        profilemodel.h
        skeletonmodel.cpp
        skeletonmodel.h
    QML_FILES
        ActionListItem.qml
        ArtikulateDrawer.qml
        CourseConfigurationPage.qml
        EditCoursePage.qml
        Editor.qml
        EditorCourseSelectionPage.qml
        EditorDrawer.qml
        EditorSkeletonSelectionPage.qml
        Main.qml
        PhonemeUnitSelector.qml
        PhraseEditor.qml
        PhraseEditorEditStateComponent.qml
        PhraseEditorSoundComponent.qml
        PhraseEditorTypeComponent.qml
        ProfileDetailsItem.qml
        ProfileSelector.qml
        ProfileSettingsPage.qml
        ProfileUserImageItem.qml
        ProfileUserItem.qml
        RepositoryConfigurationPage.qml
        SoundPlayer.qml
        SoundRecorder.qml
        TrainerCourseStatistics.qml
        TrainingPage.qml
        WelcomePage.qml
)

target_link_libraries(artikluate_backing
    PUBLIC Qt::Gui KF6::ItemModels artikulatecore
)
