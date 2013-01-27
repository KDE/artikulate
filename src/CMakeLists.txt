include(MacroLogFeature)

find_package(KDeclarative REQUIRED)

# TODO the following selfmade cmake modules are only for now
#      remove them when official ones are available
find_package(QtMobility)
find_package(QtMultimedia)

add_subdirectory(qml)

# set include directories
include_directories(
    ${QT_INCLUDE}
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QTMOBILITY_INCLUDE_DIR}
    ${QTMULTIMEDIA_INCLUDE_DIR}
)

# set the source code files from which ArtiKulate is compiled
set(artikulate_SRCS
    main.cpp
    mainwindow.cpp
    editorview.cpp
    application.cpp

    core/course.cpp
    core/resourcemanager.cpp
    core/language.cpp
    core/phrase.cpp
    core/tag.cpp
    core/taggroup.cpp
    core/unit.cpp

    models/coursemodel.cpp
    models/languagemodel.cpp
    models/unitmodel.cpp
    models/phrasemodel.cpp

    declarativeitems/applicationbackground.cpp
)
# message("${QT_INCLUDE_DIR}")

kde4_add_executable(artikulate ${artikulate_SRCS})

target_link_libraries( artikulate
    ${KDECLARATIVE_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTXMLPATTERNS_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${PHONON_LIBS}
    QtMultimediaKit
)

install(TARGETS artikulate ${INSTALL_TARGETS_DEFAULT_ARGS})
