add_library(artikluate_backing STATIC)

qt_add_qml_module(
    artikluate_backing
    URI org.kde.artikulate
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

target_link_libraries(artikluate_backing)
