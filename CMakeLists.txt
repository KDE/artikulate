 ######
 #  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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
 ######

project(artikulate)

set ( CMAKE_MODULE_PATH
     ${CMAKE_SOURCE_DIR}/cmake/modules
     ${CMAKE_MODULE_PATH}
)

find_package(KDE4 4.9.0 REQUIRED)
include(KDE4Defaults)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${QT4_INCLUDES})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

# subdirectories to build
add_subdirectory(data)
add_subdirectory(schemes)
#add_subdirectory(doc)
add_subdirectory(src)
#add_subdirectory(sounds)
#add_subdirectory(images)
add_subdirectory(icons)

# Build Tests if KDE_TEST is on
if(KDE4_BUILD_TESTS)
    enable_testing()
    add_subdirectory(autotests)
endif(KDE4_BUILD_TESTS)

# files to install in the artikulate project root directory
install( PROGRAMS artikulate.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )

macro_display_feature_log()
