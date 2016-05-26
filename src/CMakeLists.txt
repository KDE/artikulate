###
#  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

ecm_setup_version(0.6.50 "${CMAKE_CURRENT_BINARY_DIR}/version.h"
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
    core/course.cpp
    core/resourcemanager.cpp
    core/language.cpp
    core/phrase.cpp
    core/phoneme.cpp
    core/phonemegroup.cpp
    core/unit.cpp
    core/skeleton.cpp
    core/editorsession.cpp
    core/trainingsession.cpp
    core/resources/resourceinterface.cpp
    core/resources/languageresource.cpp
    core/resources/courseresource.cpp
    core/resources/skeletonresource.cpp
    core/player.cpp
    core/recorder.cpp

    qmlcontrols/iconitem.cpp
    qmlcontrols/imagetexturescache.cpp
    qmlcontrols/managedtexturenode.cpp

    artikulate_debug.cpp
)

kconfig_add_kcfg_files (artikulateCore_SRCS settings.kcfgc)
add_library(artikulatecore SHARED ${artikulateCore_SRCS})
generate_export_header(artikulatecore BASE_NAME artikulatecore)
target_link_libraries(artikulatecore
    LINK_PUBLIC
        artikulatelearnerprofile
        artikulatesound
        Qt5::XmlPatterns
        Qt5::Quick
        KF5::Archive
        KF5::ConfigGui
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
    DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
)
install(FILES artikulate.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR})

# set the source code files from which Artikulate is compiled
set(artikulate_SRCS
    main.cpp
    mainwindow.cpp
    application.cpp
    artikulate_debug.cpp

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

    ui/sounddevicedialogpage.cpp
    ui/appearencedialogpage.cpp
    ui/resourcesdialogpage.cpp
)
ki18n_wrap_ui (artikulate_SRCS
    ui/resourcesdialogpage.ui
    ui/sounddevicedialogpage.ui
    ui/appearencedialogpage.ui
)
qt5_add_resources(artikulate_SRCS qml/qml.qrc)
kconfig_add_kcfg_files (artikulate_SRCS settings.kcfgc)

set(artikulate_editor_SRCS
    main_editor.cpp
    mainwindow_editor.cpp
    application.cpp
    artikulate_debug.cpp

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

    ui/sounddevicedialogpage.cpp
    ui/appearencedialogpage.cpp
    ui/resourcesdialogpage.cpp
    ui/exportghnsdialog.cpp
)
ki18n_wrap_ui (artikulate_editor_SRCS
    ui/appearencedialogpage.ui
    ui/exportghnsdialog.ui
    ui/resourcesdialogpage.ui
    ui/sounddevicedialogpage.ui
)
qt5_add_resources(artikulate_editor_SRCS qml/qml.qrc)
kconfig_add_kcfg_files (artikulate_editor_SRCS settings.kcfgc)

# executables
add_executable(artikulate ${artikulate_SRCS})
target_link_libraries(artikulate
    LINK_PUBLIC
        artikulatelearnerprofile
        artikulatesound
        artikulatecore
        Qt5::Qml
        Qt5::Quick
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
        Qt5::Qml
        Qt5::Quick
        Qt5::QuickWidgets
        KF5::Crash
        KF5::NewStuff
        KF5::XmlGui
)

install(FILES artikulate.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(TARGETS artikulate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS artikulate_editor ${INSTALL_TARGETS_DEFAULT_ARGS})
