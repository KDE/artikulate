# SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

find_package(Qt5Multimedia REQUIRED)

set(qtmultimediabackend_SRCS
    qtmultimediabackend.cpp
    qtmultimediacapturebackend.cpp
    qtmultimediaoutputbackend.cpp
    ../libsound_debug.cpp
)

add_library(qtmultimediabackend MODULE ${qtmultimediabackend_SRCS})

target_link_libraries(
    qtmultimediabackend
    LINK_PUBLIC
        artikulatesound
        Qt5::Multimedia
        KF5::CoreAddons
)

install(TARGETS qtmultimediabackend DESTINATION ${KDE_INSTALL_PLUGINDIR}/artikulate/libsound)
