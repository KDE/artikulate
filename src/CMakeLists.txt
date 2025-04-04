# SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

ecm_setup_version(${ARTIKULATE_VERSION}
    VARIABLE_PREFIX ARTIKULATE
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/ArtikulateConfigVersion.cmake"
)

# set include directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${artikulate_SOURCE_DIR}
)

# set the source code files from which Artikulate is compiled
set(artikulateCore_SRCS
    application.cpp
    artikulate_debug.cpp
    core/xmlschema.cpp
    core/xmlschema.h
    core/xmlschemavalidator.cpp
    core/xmlschemavalidator.h
    core/icourse.h
    core/ieditablecourse.h
    core/ieditablephrase.h
    core/ieditablerepository.h
    core/ieditableunit.h
    core/ilanguage.h
    core/iphrase.h
    core/iunit.h
    core/iresourcerepository.h
    core/resourcerepository.cpp
    core/contributorrepository.cpp
    core/language.cpp
    core/phrase.cpp
    core/phoneme.cpp
    core/phonemegroup.cpp
    core/unit.cpp
    core/resources/courseparser.cpp
    core/resources/courseresource.cpp
    core/resources/editablecourseresource.cpp
    core/resources/skeletonresource.cpp
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
        Qt::Qml
        Qt::Quick
        KF6::Archive
        KF6::ConfigGui
        LibXml2::LibXml2
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
target_include_directories(artikulatecore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
install(
    TARGETS artikulatecore
    LIBRARY NAMELINK_SKIP
    DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
install(FILES artikulate.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})

add_subdirectory(org/kde/artikulate)

# set the source code files from which Artikulate is compiled
set(artikulate_SRCS
    main.cpp
    mainwindow.cpp
    artikulate_debug.cpp
)
qt_add_resources(artikulate_SRCS resources.qrc)
qt_add_resources(artikulate_SRCS ../data/languages.qrc)
kconfig_add_kcfg_files (artikulate_SRCS settings.kcfgc)

set(artikulate_editor_SRCS
    main_editor.cpp
    mainwindow_editor.cpp
    artikulate_debug.cpp
)
qt_add_resources(artikulate_editor_SRCS resources.qrc)
qt_add_resources(artikulate_editor_SRCS ../data/languages.qrc)
kconfig_add_kcfg_files (artikulate_editor_SRCS settings.kcfgc)

# executables
add_executable(artikulate ${artikulate_SRCS})
target_link_libraries(artikulate
    LINK_PUBLIC
        artikulatelearnerprofile
        artikulatecore
        Qt::Qml
        Qt::Quick
        KF6::Crash
        KF6::NewStuffCore
        KF6::XmlGui
    LINK_PRIVATE
        artikluate_backing
        artikluate_backingplugin
)

qt6_add_resources(artikulate_editor_SRCS editor.qrc)
add_executable(artikulate_editor ${artikulate_editor_SRCS})
target_link_libraries(artikulate_editor
    LINK_PUBLIC
        artikulatecore
        Qt::Qml
        Qt::Quick
        KF6::Crash
        KF6::NewStuffCore
    LINK_PRIVATE
        artikluate_backing
        artikluate_backingplugin
)

install(FILES artikulate.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(TARGETS artikulate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS artikulate_editor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
