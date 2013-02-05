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

include(MacroLogFeature)

find_package(KDeclarative REQUIRED)

# TODO the following selfmade cmake modules are only for now
#      remove them when official ones are available
find_package(QtMobility)
find_package(QtMultimedia)

add_subdirectory(qml)

# set include directories
include_directories(
    ${QT_INCLUDE}
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QTMOBILITY_INCLUDE_DIR}
    ${QTMULTIMEDIA_INCLUDE_DIR}
)

# set the source code files from which ArtiKulate is compiled
set(artikulateCore_SRCS
    core/course.cpp
    core/resourcemanager.cpp
    core/language.cpp
    core/phrase.cpp
    core/tag.cpp
    core/taggroup.cpp
    core/unit.cpp
)

kde4_add_library(artikulatecore SHARED ${artikulateCore_SRCS})
target_link_libraries(artikulatecore
    ${KDECLARATIVE_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTXMLPATTERNS_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    QtMultimediaKit
)
set_target_properties(artikulatecore
    PROPERTIES VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)
install(TARGETS artikulatecore ${INSTALL_TARGETS_DEFAULT_ARGS})

# set the source code files from which ArtiKulate is compiled
set(artikulate_SRCS
    main.cpp
    mainwindow.cpp
    editorview.cpp
    application.cpp

    models/coursemodel.cpp
    models/languagemodel.cpp
    models/unitmodel.cpp
    models/phrasemodel.cpp

    declarativeitems/applicationbackground.cpp
)
kde4_add_executable(artikulate ${artikulate_SRCS})
target_link_libraries(artikulate
    artikulatecore
    ${KDECLARATIVE_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTXMLPATTERNS_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    QtMultimediaKit
)

install(TARGETS artikulate ${INSTALL_TARGETS_DEFAULT_ARGS})
