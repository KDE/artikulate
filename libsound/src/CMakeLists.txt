# SPDX-FileCopyrightText: 2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

# enable exceptions for this library
kde_enable_exceptions()

set(sound_LIB_SRCS
    backendinterface.cpp
    capturedevicecontroller.cpp
    outputdevicecontroller.cpp
    capturebackendinterface.cpp
    outputbackendinterface.cpp
    libsound_debug.cpp
)

add_library(artikulatesound SHARED ${sound_LIB_SRCS})
generate_export_header(artikulatesound BASE_NAME libsound)

target_link_libraries(
    artikulatesound
    LINK_PUBLIC
        KF5::CoreAddons
        KF5::I18n
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
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

if (BUILD_GSTREAMER_PLUGIN)
    ecm_optional_add_subdirectory(qtgstreamerbackend)
endif()
if (BUILD_QTMULTIMEDIA_PLUGIN)
    ecm_optional_add_subdirectory(qtmultimediabackend)
endif()
