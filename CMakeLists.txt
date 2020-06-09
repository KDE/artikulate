# Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

project(artikulate)

cmake_minimum_required(VERSION 3.5.0)
set(QT_MIN_VERSION "5.11.0")
set(KF5_MIN_VERSION "5.64.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
find_package(KF5DocTools)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEClangFormat)
include(ECMAddTests)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMOptionalAddSubdirectory)
include(ECMSetupVersion)
include(ECMEnableSanitizers)
include(FeatureSummary)
include(GenerateExportHeader)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Widgets
    Sql
    XmlPatterns
    Qml
    Quick
    Test
)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    Config
    Crash
    I18n
    NewStuff
    Kirigami2
)

# options
option(BUILD_QTMULTIMEDIA_PLUGIN "Build QtMultimedia sound backend" ON)
option(BUILD_GSTREAMER_PLUGIN "Build GStreamer sound backend" OFF)

add_definitions(
    -DQT_NO_URL_CAST_FROM_STRING
)

# subdirectories to build
ecm_optional_add_subdirectory(data)
ecm_optional_add_subdirectory(doc)
ecm_optional_add_subdirectory(src)
ecm_optional_add_subdirectory(icons)
ecm_optional_add_subdirectory(libsound)
ecm_optional_add_subdirectory(liblearnerprofile)
ecm_optional_add_subdirectory(autotests)

# files to install in the artikulate project root directory
install(PROGRAMS org.kde.artikulate.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES org.kde.artikulate.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
