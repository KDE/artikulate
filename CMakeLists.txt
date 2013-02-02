# ArtiKulate CMakeLists.txt file
#

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
#add_subdirectory(icons)

# Build Tests if KDE_TEST is on
if(KDE4_BUILD_TESTS)
    enable_testing()
    add_subdirectory(autotests)
endif(KDE4_BUILD_TESTS)

# files to install in the artikulate project root directory
install( PROGRAMS artikulate.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )

macro_display_feature_log()
