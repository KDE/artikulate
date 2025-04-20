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
add_executable(test_resourcerepository)
target_sources(test_resourcerepository PRIVATE
    resourcerepository/test_resourcerepository.cpp
    resourcerepository/test_resourcerepository.h
)
add_language_resources(test_resourcerepository)
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
    trainingsession/test_trainingsession.h
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/editorsession.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/isessionactions.h
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/trainingsession.cpp
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
    ${CMAKE_SOURCE_DIR}/src/artikulate_debug.cpp
    editorsession/test_editorsession.cpp
    editorsession/test_editorsession.h
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
add_executable(test_courseresource)
target_sources(test_courseresource PRIVATE
    courseresource/test_courseresource.cpp
    courseresource/test_courseresource.h
    ../mocks/resourcerepositorystub.cpp
    ../mocks/languagestub.cpp
)
add_language_resources(test_courseresource)
target_link_libraries(test_courseresource
    artikulatecore
    Qt::Test
)
add_test(NAME test_courseresource COMMAND test_courseresource)
ecm_mark_as_test(test_courseresource)


# test skeleton resource class
add_executable(test_skeletonresource)
target_sources(test_skeletonresource PRIVATE
    skeletonresource/test_skeletonresource.cpp
    skeletonresource/test_skeletonresource.h
    ../mocks/resourcerepositorystub.cpp
    ../mocks/languagestub.cpp
)
add_language_resources(test_skeletonresource)
target_link_libraries(test_skeletonresource
    artikulatecore
    Qt::Test
)
add_test(NAME test_skeletonresource COMMAND test_skeletonresource)
ecm_mark_as_test(test_skeletonresource)


# test course resource class
add_executable(test_unit)
target_sources(test_unit PRIVATE
    unit/test_unit.cpp
    unit/test_unit.h
)
add_language_resources(test_unit)
target_link_libraries(test_unit
    artikulatecore
    Qt::Test
)
add_test(NAME test_unit COMMAND test_unit)
ecm_mark_as_test(test_unit)


# test editable course resource class
add_executable(test_editablecourseresource)
target_sources(test_editablecourseresource PRIVATE
    editablecourseresource/test_editablecourseresource.cpp
    editablecourseresource/test_editablecourseresource.h
    ../mocks/resourcerepositorystub.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
add_language_resources(test_editablecourseresource)
add_testdata_resources(test_editablecourseresource)
target_link_libraries(test_editablecourseresource
    artikulatecore
    Qt::Test
)
add_test(NAME test_editablecourseresource COMMAND test_editablecourseresource)
ecm_mark_as_test(test_editablecourseresource)


# test course model class
add_executable(test_coursemodel)
target_sources(test_coursemodel PRIVATE
    coursemodel/test_coursemodel.cpp
    coursemodel/test_coursemodel.h
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/coursemodel.cpp
    ${CMAKE_SOURCE_DIR}/src/artikulate_debug.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
add_language_resources(test_coursemodel)
add_testdata_resources(test_coursemodel)
target_link_libraries(test_coursemodel
    artikulatecore
    Qt::Test
)
add_test(NAME test_coursemodel COMMAND test_coursemodel)
ecm_mark_as_test(test_coursemodel)


# test skeleton model class
add_executable(test_skeletonmodel)
target_sources(test_skeletonmodel PRIVATE
    skeletonmodel/test_skeletonmodel.cpp
    skeletonmodel/test_skeletonmodel.h
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/skeletonmodel.cpp
    ../mocks/editablerepositorystub.cpp
    ../mocks/editablecoursestub.cpp
    ../mocks/languagestub.cpp
)
add_language_resources(test_skeletonmodel)
add_testdata_resources(test_skeletonmodel)
target_link_libraries(test_skeletonmodel
    artikulatecore
    Qt::Test
)
add_test(NAME test_skeletonmodel COMMAND test_skeletonmodel)
ecm_mark_as_test(test_skeletonmodel)

# test drawer course tree model class
add_executable(test_drawercoursetreemodel)
target_sources(test_drawercoursetreemodel PUBLIC
    drawercoursetreemodel/test_drawercoursetreemodel.cpp
    drawercoursetreemodel/test_drawercoursetreemodel.h
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/drawercoursetreemodel.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/editorsession.cpp
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/isessionactions.h
    ${CMAKE_SOURCE_DIR}/src/org/kde/artikulate/trainingsession.cpp
    ${CMAKE_SOURCE_DIR}/src/artikulate_debug.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
target_link_libraries(test_drawercoursetreemodel
    artikulatecore
    Qt::Test
)
add_test(NAME test_drawercoursetreemodel COMMAND test_drawercoursetreemodel)
ecm_mark_as_test(test_drawercoursetreemodel)
