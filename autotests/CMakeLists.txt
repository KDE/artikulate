# ArtiKulate CMakeLists.txt file
#

include_directories(
    ../src/
    ${CMAKE_CURRENT_BINARY_DIR}
)

# basic tests for audio capability
set (TestAudio_SRCS
    testaudio.cpp
)
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
