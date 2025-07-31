# SPDX-FileCopyrightText: 2013-2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16.0)
set(QT_MIN_VERSION "6.8.0")
set(KF_MIN_VERSION "6.0.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.84.0")

# generate QML LS integration
set(QT_QML_GENERATE_QMLLS_INI ON)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "25")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "0")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")
set (ARTIKULATE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(artikulate VERSION ${ARTIKULATE_VERSION})

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
find_package(KF6DocTools)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )
find_package(LibXml2 REQUIRED)

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
include(ECMDeprecationSettings)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Sql
    Multimedia
    Qml
    Quick
    Test
    Xml
)
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)
qt_policy(SET QTP0005 NEW)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    Config
    Crash
    I18n
    ItemModels
    NewStuff
    Kirigami
)

ecm_set_disabled_deprecation_versions(QT 6.5.0
     KF 6.0.0
    )

add_definitions(
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_KEYWORDS
    -DQT_NO_FOREACH
)

# subdirectories to build
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(liblearnerprofile)
add_subdirectory(autotests)

# files to install in the artikulate project root directory
ki18n_install(po)
if (KF6DocTools_FOUND)
    kdoctools_install(po)
endif()
install(PROGRAMS org.kde.artikulate.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.artikulate.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
