###
#  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
#  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
#  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
#  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
#  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
###

project(artikulate)

cmake_minimum_required(VERSION 2.8.12)
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
find_package(KF5DocTools)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(ECMAddTests)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)
include(ECMOptionalAddSubdirectory)
include(ECMSetupVersion)
include(FeatureSummary)

find_package(Qt5 REQUIRED COMPONENTS
    Widgets
    Sql
    XmlPatterns
    Quick
    QuickWidgets
    Test
)
find_package(KF5 CONFIG REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    Declarative
    I18n
    KIO
    NewStuff
    WidgetsAddons
)

# subdirectories to build
ecm_optional_add_subdirectory(data)
ecm_optional_add_subdirectory(schemes)
ecm_optional_add_subdirectory(doc)
ecm_optional_add_subdirectory(src)
ecm_optional_add_subdirectory(sounds)
ecm_optional_add_subdirectory(images)
ecm_optional_add_subdirectory(icons)
ecm_optional_add_subdirectory(libsound)
ecm_optional_add_subdirectory(liblearnerprofile)
ecm_optional_add_subdirectory(autotests)

# files to install in the artikulate project root directory
install(PROGRAMS artikulate.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES artikulate.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
