set(artikulate_qml_files
    Main.qml
    Editor.qml
    HomeScreen.qml
    TrainingScreen.qml

    CourseSelector.qml
    LanguageSelector.qml
    UnitSelector.qml
)

install(FILES ${artikulate_qml_files} DESTINATION ${DATA_INSTALL_DIR}/artikulate/qml)
