###
# Copyright 2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
###

find_package(Qt5GStreamer 1.2.0 REQUIRED)

# enable exceptions for this library
kde_enable_exceptions()

include_directories(
    ${QTGSTREAMER_INCLUDE_DIR}
)

set(sound_LIB_SRCS
    capturedevicecontroller.cpp
    outputdevicecontroller.cpp
    qtgstreamercapturebackend.cpp
    qtgstreameroutputbackend.cpp
)

add_library(artikulatesound SHARED ${sound_LIB_SRCS})
generate_export_header(artikulatesound BASE_NAME libsound)

target_link_libraries(
    artikulatesound
    LINK_PUBLIC
        ${QTGLIB_LIBRARIES}
        ${QTGLIB_LIBRARY}
        ${QTGSTREAMER_LIBRARIES}
        KF5::I18n
)

set_target_properties(
    artikulatesound
    PROPERTIES
        VERSION ${GENERIC_LIB_VERSION}
        SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS artikulatesound
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)
