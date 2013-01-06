include(MacroLogFeature)

find_package(KDeclarative REQUIRED)

# set include directories
include_directories(
    ${QT_INCLUDE}
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# set the source code files from which ArtiKulate is compiled
set(artikulate_SRCS
    main.cpp
)

kde4_add_executable(artikulate ${artikulate_SRCS})

target_link_libraries( artikulate
    ${KDECLARATIVE_LIBRARIES}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTXMLPATTERNS_LIBRARY}
    ${QT_QTSQL_LIBRARY})

install(TARGETS artikulate ${INSTALL_TARGETS_DEFAULT_ARGS})

