# SPDX-FileCopyrightText: 2013-2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.5.0)
set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.82.0")

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "22")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "0")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")
set (ARTIKULATE_VERSION "1.0.0.${RELEASE_SERVICE_COMPACT_VERSION}")

project(artikulate VERSION ${ARTIKULATE_VERSION})

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
find_package(KF5DocTools)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

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

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS
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
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(libsound)
add_subdirectory(liblearnerprofile)
add_subdirectory(autotests)

# files to install in the artikulate project root directory
ki18n_install(po)
if (KF5DocTools_FOUND)
    kdoctools_install(po)
endif()
install(PROGRAMS org.kde.artikulate.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.artikulate.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
