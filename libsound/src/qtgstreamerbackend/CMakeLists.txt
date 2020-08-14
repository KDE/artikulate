# SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

find_package(Qt5GStreamer 1.2.0 REQUIRED)
include_directories(
    ${QTGSTREAMER_INCLUDES}
)

set(qtgstreamerbackend_SRCS
    qtgstreamerbackend.cpp
    qtgstreamercapturebackend.cpp
    qtgstreameroutputbackend.cpp
    ../libsound_debug.cpp
)

add_library(qtgstreamerbackend MODULE ${qtgstreamerbackend_SRCS})

target_link_libraries(
    qtgstreamerbackend
    LINK_PUBLIC
        artikulatesound
        KF5::CoreAddons
        ${QTGLIB_LIBRARIES}
        ${QTGLIB_LIBRARY}
        ${QTGSTREAMER_LIBRARIES}
)

install(TARGETS qtgstreamerbackend DESTINATION ${PLUGIN_INSTALL_DIR}/artikulate/libsound)
