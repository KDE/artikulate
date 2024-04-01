# SPDX-FileCopyrightText: 2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

# enable exceptions for this library
kde_enable_exceptions()

set(sound_LIB_SRCS
    capturedevicecontroller.cpp
    outputdevicecontroller.cpp
    capturebackendinterface.cpp
    outputbackendinterface.cpp
    libsound_debug.cpp
    qtmultimediabackend/qtmultimediacapturebackend.cpp
    qtmultimediabackend/qtmultimediaoutputbackend.cpp
)

add_library(artikulatesound SHARED ${sound_LIB_SRCS})
generate_export_header(artikulatesound BASE_NAME libsound)

target_link_libraries(
    artikulatesound
    LINK_PUBLIC
        KF6::CoreAddons
        KF6::I18n
        Qt::Multimedia
)
# internal library without any API or ABI guarantee
set(GENERIC_LIB_VERSION "0")
set(GENERIC_LIB_SOVERSION "0")
set_target_properties(
    artikulatesound PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS artikulatesound
    LIBRARY NAMELINK_SKIP
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
