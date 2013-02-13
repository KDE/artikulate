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

include_directories(
    ../src/
    ${CMAKE_CURRENT_BINARY_DIR}
)

# basic tests for audio capability
set (TestAudio_SRCS
    testaudio.cpp
)
file(COPY ../schemes/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/schemes) # copy test files
file(COPY ../data/courses DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/courses) # copy test files
kde4_add_executable(TestAudio ${TestAudio_SRCS} )
target_link_libraries(TestAudio
    artikulatecore
    ${QT_QTTEST_LIBRARY}
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
    ${PHONON_LIBS}
    QtMultimediaKit
)
add_test(TestAudio TestAudio)


# basic tests course files (input/output)
set (TestCourseFiles_SRCS
    testcoursefiles.cpp
)
kde4_add_executable(TestCourseFiles ${TestCourseFiles_SRCS} )
target_link_libraries(TestCourseFiles
    artikulatecore
    ${QT_QTTEST_LIBRARY}
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
)
add_test(TestCourseFiles TestCourseFiles)
