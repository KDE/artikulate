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
set(ARTIKULATE_VERSION_MAJOR 0)
set(ARTIKULATE_VERSION_MINOR 3)
set(ARTIKULATE_VERSION_PATCH 0)

find_package(KDeclarative REQUIRED)
find_package(QtMobility COMPONENTS MultimediaKit QUIET)
find_package(QtGStreamer QUIET)

# print information about built recording backends
message("----------------------------------")
message("Building following recording backends:")
if (QTGSTREAMER_FOUND)
    message("  * QtGStreamer recording backend")
endif(QTGSTREAMER_FOUND)
if (QTMOBILITY_FOUND)
    message("  * QtMultimedia recording backend (default)")
endif(QTMOBILITY_FOUND)
if (NOT QTGSTREAMER_FOUND AND NOT QTMOBILITY_FOUND)
    message(FATAL_ERROR "No recording backend built: either QtGStreamer or QtMultimedia are required.")
endif()
message("----------------------------------")

# set config files after packages are searched
configure_file("${artikulate_SOURCE_DIR}/version.h.cmake" "${artikulate_BINARY_DIR}/version.h" @ONLY)
configure_file("${artikulate_SOURCE_DIR}/config.h.cmake" "${artikulate_BINARY_DIR}/config.h" @ONLY)


add_subdirectory(qml)

# set include directories
include_directories(
    ${QT_INCLUDE}
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
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
    core/profile.cpp
    core/skeleton.cpp
    core/trainingsession.cpp
    core/capturedevicecontroller.cpp
    core/outputdevicecontroller.cpp
    core/resources/resourceinterface.cpp
    core/resources/languageresource.cpp
    core/resources/courseresource.cpp
    core/resources/skeletonresource.cpp
    core/player.cpp
    core/recorder.cpp
    core/soundbackends/soundbackendinterface.cpp
    ui/newcoursedialog.cpp
)

set(
    artikulateCoreTargetLibs
    ${KDECLARATIVE_LIBRARIES}
    ${KDE4_KIO_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTXMLPATTERNS_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${PHONON_LIBS}
)

set(
    artikulateTargetLibs
    artikulatecore
    artikulatelearnerprofile
    ${KDECLARATIVE_LIBRARIES}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KNEWSTUFF3_LIBS}
)

# select backend and add files
if (QTMOBILITY_FOUND)
    include_directories(
        ${QTMOBILITY_INCLUDE_DIR}
        ${QTMOBILITY_MULTIMEDIAKIT_INCLUDE_DIR}
    )
    set(artikulateCore_SRCS
        ${artikulateCore_SRCS}
        core/soundbackends/qtmultimediabackend.cpp
    )
    set(artikulateCoreTargetLibs
        ${artikulateCoreTargetLibs}
        ${QTMOBILITY_MULTIMEDIAKIT_LIBRARY}
    )
    set(artikulateeTargetLibs
        ${artikulateTargetLibs}
        ${QTMOBILITY_MULTIMEDIAKIT_LIBRARY}
    )
endif(QTMOBILITY_FOUND)

if(QTGSTREAMER_FOUND)
    include_directories(
        ${QTGSTREAMER_INCLUDE_DIR}
    )
    set(artikulateCore_SRCS
        ${artikulateCore_SRCS}
        core/soundbackends/qtgstreamerbackend.cpp
    )
    set(artikulateCoreTargetLibs
        ${artikulateCoreTargetLibs}
        ${QTGLIB_LIBRARIES}
        ${QTGSTREAMER_LIBRARIES}
    )
    set(artikulateeTargetLibs
        ${artikulateTargetLibs}
        ${QTGLIB_LIBRARIES}
        ${QTGSTREAMER_LIBRARIES}
    )
endif(QTGSTREAMER_FOUND)

# compile UI files
kde4_add_ui_files(artikulateCore_SRCS
    ui/newcoursedialog.ui
)
kde4_add_kcfg_files(artikulateCore_SRCS settings.kcfgc)
kde4_add_library(artikulatecore SHARED ${artikulateCore_SRCS})
target_link_libraries(artikulatecore
    LINK_PRIVATE
        artikulatelearnerprofile
        ${artikulateCoreTargetLibs}
)
set_target_properties(
    artikulatecore
    PROPERTIES
        VERSION ${GENERIC_LIB_VERSION}
        SOVERSION ${GENERIC_LIB_SOVERSION}
)
install(
    TARGETS artikulatecore
    DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
)
install( FILES artikulate.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )

# set the source code files from which Artikulate is compiled
set(artikulate_SRCS
    main.cpp
    mainwindow.cpp
    application.cpp

    models/coursemodel.cpp
    models/coursefiltermodel.cpp
    models/languagemodel.cpp
    models/languageresourcemodel.cpp
    models/learningprogressmodel.cpp
    models/unitmodel.cpp
    models/unitfiltermodel.cpp
    models/phrasemodel.cpp
    models/phrasefiltermodel.cpp
    models/phonememodel.cpp
    models/phonemegroupmodel.cpp
    models/phonemeunitmodel.cpp
    models/profilemodel.cpp
    models/skeletonmodel.cpp

    ui/sounddevicedialogpage.cpp
    ui/appearencedialogpage.cpp
    ui/resourcesdialogpage.cpp

    declarativeitems/applicationbackground.cpp
)
kde4_add_ui_files(artikulate_SRCS
    ui/resourcesdialogpage.ui
    ui/sounddevicedialogpage.ui
    ui/appearencedialogpage.ui
)
kde4_add_kcfg_files(artikulate_SRCS settings.kcfgc)
kde4_add_executable(artikulate ${artikulate_SRCS})
target_link_libraries(artikulate
    LINK_PRIVATE
        artikulatelearnerprofile
        ${artikulateTargetLibs}
)

install(FILES artikulate.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(TARGETS artikulate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES artikulateui.rc DESTINATION  ${DATA_INSTALL_DIR})
