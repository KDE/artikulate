# SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(
    ../../src/
    ../../
    ../mocks/
    ${CMAKE_CURRENT_BINARY_DIR}
)

# copy test data
file(COPY ../testdata/courses/de.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/courses/de/) # copy test files
file(COPY ../testdata/courses/fr.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/courses/fr/) # copy test files
file(COPY ../testdata/contributorrepository/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/contributorrepository/) # copy test files

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# repository tests
set(TestResourceRepository_SRCS resourcerepository/test_resourcerepository.cpp)
qt6_add_resources(TestResourceRepository_SRCS ../../data/languages.qrc)
add_executable(test_resourcerepository ${TestResourceRepository_SRCS})
target_link_libraries(test_resourcerepository
    artikulatecore
    Qt::Test
)
add_test(NAME test_resourcerepository COMMAND test_resourcerepository)
ecm_mark_as_test(test_resourcerepository)


# training session tests
add_executable(test_trainingsession)
target_sources(test_trainingsession PUBLIC
    trainingsession/test_trainingsession.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/editorsession.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/isessionactions.h
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/trainingsession.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/trainingaction.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/trainingactionicon.cpp
    ${CMAKE_SOURCE_DIR}/src/artikulate_debug.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
target_link_libraries(test_trainingsession
    artikulatecore
    Qt::Test
)
add_test(NAME test_trainingsession COMMAND test_trainingsession)
ecm_mark_as_test(test_trainingsession)


# editor session tests
add_executable(test_editorsession)
target_sources(test_editorsession PUBLIC
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/editorsession.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/isessionactions.h
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/trainingsession.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/trainingaction.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/trainingactionicon.cpp
    ${CMAKE_SOURCE_DIR}/src/artikulate_debug.cpp
    editorsession/test_editorsession.cpp
    ../mocks/editablerepositorystub.cpp
    ../mocks/editablecoursestub.cpp
    ../mocks/languagestub.cpp
)
target_link_libraries(test_editorsession
    artikulatecore
    Qt::Test
)
add_test(NAME test_editorsession COMMAND test_editorsession)
ecm_mark_as_test(test_editorsession)


# test course resource class
set(TestCourseResource_SRCS
    courseresource/test_courseresource.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/languagestub.cpp
)
qt6_add_resources(TestCourseResource_SRCS ../../data/languages.qrc)
add_executable(test_courseresource ${TestCourseResource_SRCS} )
target_link_libraries(test_courseresource
    artikulatecore
    Qt::Test
)
add_test(NAME test_courseresource COMMAND test_courseresource)
ecm_mark_as_test(test_courseresource)


# test skeleton resource class
set(TestSkeletonResource_SRCS
    skeletonresource/test_skeletonresource.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/languagestub.cpp
)
qt6_add_resources(TestSkeletonResource_SRCS ../../data/languages.qrc)
add_executable(test_skeletonresource ${TestSkeletonResource_SRCS} )
target_link_libraries(test_skeletonresource
    artikulatecore
    Qt::Test
)
add_test(NAME test_skeletonresource COMMAND test_skeletonresource)
ecm_mark_as_test(test_skeletonresource)


# test course resource class
set(TestUnit_SRCS
    unit/test_unit.cpp
)
qt6_add_resources(TestUnit_SRCS ../../data/languages.qrc)
add_executable(test_unit ${TestUnit_SRCS} )
target_link_libraries(test_unit
    artikulatecore
    Qt::Test
)
add_test(NAME test_unit COMMAND test_unit)
ecm_mark_as_test(test_unit)


# test editable course resource class
set(TestEditableCourseResource_SRCS
    editablecourseresource/test_editablecourseresource.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
qt6_add_resources(TestEditableCourseResource_SRCS ../../data/languages.qrc)
qt6_add_resources(TestEditableCourseResource_SRCS ../testdata/testdata.qrc)
add_executable(test_editablecourseresource ${TestEditableCourseResource_SRCS} )
target_link_libraries(test_editablecourseresource
    artikulatecore
    Qt::Test
)
add_test(NAME test_editablecourseresource COMMAND test_editablecourseresource)
ecm_mark_as_test(test_editablecourseresource)


# test course model class
set(TestCourseModel_SRCS
    coursemodel/test_coursemodel.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/coursemodel.cpp
    ${CMAKE_SOURCE_DIR}/src/artikulate_debug.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
qt6_add_resources(TestCourseModel_SRCS ../../data/languages.qrc)
qt6_add_resources(TestCourseModel_SRCS ../testdata/testdata.qrc)
add_executable(test_coursemodel ${TestCourseModel_SRCS})
target_link_libraries(test_coursemodel
    artikulatecore
    Qt::Test
)
add_test(NAME test_coursemodel COMMAND test_coursemodel)
ecm_mark_as_test(test_coursemodel)


# test skeleton model class
set(TestSkeletonModel_SRCS
    skeletonmodel/test_skeletonmodel.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/skeletonmodel.cpp
    ../mocks/editablerepositorystub.cpp
    ../mocks/editablecoursestub.cpp
    ../mocks/languagestub.cpp
)
qt6_add_resources(TestSkeletonModel_SRCS ../../data/languages.qrc)
qt6_add_resources(TestSkeletonModel_SRCS ../testdata/testdata.qrc)
add_executable(test_skeletonmodel ${TestSkeletonModel_SRCS})
target_link_libraries(test_skeletonmodel
    artikulatecore
    Qt::Test
)
add_test(NAME test_skeletonmodel COMMAND test_skeletonmodel)
ecm_mark_as_test(test_skeletonmodel)
