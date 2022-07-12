# SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

ecm_setup_version(${ARTIKULATE_VERSION}
    VARIABLE_PREFIX ARTIKULATE
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/ArtikulateConfigVersion.cmake"
)

ecm_optional_add_subdirectory(qml)

# set include directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${artikulate_SOURCE_DIR}
)

# set the source code files from which Artikulate is compiled
set(artikulateCore_SRCS
    application.cpp
    artikulate_debug.cpp
    core/icourse.h
    core/ieditablecourse.h
    core/ieditablephrase.h
    core/ieditablerepository.h
    core/ieditableunit.h
    core/ilanguage.h
    core/iphrase.h
    core/isessionactions.h
    core/iunit.h
    core/iresourcerepository.h
    core/drawertrainingactions.cpp
    core/resourcerepository.cpp
    core/contributorrepository.cpp
    core/language.cpp
    core/phrase.cpp
    core/phoneme.cpp
    core/phonemegroup.cpp
    core/unit.cpp
    core/editorsession.cpp
    core/trainingaction.cpp
    core/trainingactionicon.cpp
    core/trainingsession.cpp
    core/resources/courseparser.cpp
    core/resources/courseresource.cpp
    core/resources/editablecourseresource.cpp
    core/resources/skeletonresource.cpp
    core/player.cpp
    core/recorder.cpp
    models/coursemodel.cpp
    models/coursefiltermodel.cpp
    models/languagemodel.cpp
    models/languageresourcemodel.cpp
#     models/learningprogressmodel.cpp //TODO must be adapted to new trainingsession
    models/unitmodel.cpp
    models/unitfiltermodel.cpp
    models/phrasemodel.cpp
    models/phraselistmodel.cpp
    models/phrasefiltermodel.cpp
    models/phonememodel.cpp
    models/phonemegroupmodel.cpp
    models/phonemeunitmodel.cpp
    models/profilemodel.cpp
    models/skeletonmodel.cpp
    qmlcontrols/iconitem.cpp
    qmlcontrols/imagetexturescache.cpp
    qmlcontrols/managedtexturenode.cpp
)

kconfig_add_kcfg_files (artikulateCore_SRCS settings.kcfgc)
add_library(artikulatecore SHARED ${artikulateCore_SRCS})
generate_export_header(artikulatecore BASE_NAME artikulatecore)
target_link_libraries(artikulatecore
    PUBLIC
        artikulatelearnerprofile
        artikulatesound
        Qt::XmlPatterns
        Qt::Qml
        Qt::Quick
        KF5::Archive
        KF5::ConfigGui
    PRIVATE
        Qt::Xml
)
# internal library without any API or ABI guarantee
set(GENERIC_LIB_VERSION "0")
set(GENERIC_LIB_SOVERSION "0")
set_target_properties(
    artikulatecore PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)
install(
    TARGETS artikulatecore
    LIBRARY NAMELINK_SKIP
    DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
install(FILES artikulate.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})

# set the source code files from which Artikulate is compiled
set(artikulate_SRCS
    main.cpp
    mainwindow.cpp
    artikulate_debug.cpp
)
qt5_add_resources(artikulate_SRCS resources.qrc)
qt5_add_resources(artikulate_SRCS ../data/languages.qrc)
kconfig_add_kcfg_files (artikulate_SRCS settings.kcfgc)

set(artikulate_editor_SRCS
    main_editor.cpp
    mainwindow_editor.cpp
    artikulate_debug.cpp
)
qt5_add_resources(artikulate_editor_SRCS resources.qrc)
qt5_add_resources(artikulate_editor_SRCS ../data/languages.qrc)
kconfig_add_kcfg_files (artikulate_editor_SRCS settings.kcfgc)

# executables
add_executable(artikulate ${artikulate_SRCS})
target_link_libraries(artikulate
    LINK_PUBLIC
        artikulatelearnerprofile
        artikulatesound
        artikulatecore
        Qt::Qml
        Qt::Quick
        KF5::Crash
        KF5::NewStuff
        KF5::XmlGui
)

qt5_add_resources(artikulate_editor_SRCS editor.qrc)
add_executable(artikulate_editor ${artikulate_editor_SRCS})
target_link_libraries(artikulate_editor
    LINK_PUBLIC
        artikulatesound
        artikulatecore
        Qt::Qml
        Qt::Quick
        KF5::Crash
        KF5::NewStuff
)

install(FILES artikulate.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(TARGETS artikulate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS artikulate_editor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
