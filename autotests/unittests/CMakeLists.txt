###
#  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
###

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
qt5_add_resources(TestResourceRepository_SRCS ../../data/languages.qrc)
add_executable(test_resourcerepository ${TestResourceRepository_SRCS})
target_link_libraries(test_resourcerepository
    artikulatecore
    Qt5::Test
)
add_test(NAME test_resourcerepository COMMAND test_resourcerepository)
ecm_mark_as_test(test_resourcerepository)


# training session tests
set(TestTrainingSession_SRCS
    trainingsession/test_trainingsession.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
add_executable(test_trainingsession ${TestTrainingSession_SRCS})
target_link_libraries(test_trainingsession
    artikulatecore
    Qt5::Test
)
add_test(NAME test_trainingsession COMMAND test_trainingsession)
ecm_mark_as_test(test_trainingsession)


# editor session tests
set(TestEditorSession_SRCS
    editorsession/test_editorsession.cpp
    ../mocks/editablerepositorystub.cpp
    ../mocks/languagestub.cpp
)
add_executable(test_editorsession ${TestEditorSession_SRCS})
target_link_libraries(test_editorsession
    artikulatecore
    Qt5::Test
)
add_test(NAME test_editorsession COMMAND test_editorsession)
ecm_mark_as_test(test_editorsession)


# test course resource class
set(TestCourseResource_SRCS
    courseresource/test_courseresource.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/languagestub.cpp
)
qt5_add_resources(TestCourseResource_SRCS ../../data/languages.qrc)
add_executable(test_courseresource ${TestCourseResource_SRCS} )
target_link_libraries(test_courseresource
    artikulatecore
    Qt5::Test
)
add_test(NAME test_courseresource COMMAND test_courseresource)
ecm_mark_as_test(test_courseresource)


# test skeleton resource class
set(TestSkeletonResource_SRCS
    skeletonresource/test_skeletonresource.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/languagestub.cpp
)
qt5_add_resources(TestSkeletonResource_SRCS ../../data/languages.qrc)
add_executable(test_skeletonresource ${TestSkeletonResource_SRCS} )
target_link_libraries(test_skeletonresource
    artikulatecore
    Qt5::Test
)
add_test(NAME test_skeletonresource COMMAND test_skeletonresource)
ecm_mark_as_test(test_skeletonresource)


# test editable course resource class
set(TestEditableCourseResource_SRCS
    editablecourseresource/test_editablecourseresource.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
qt5_add_resources(TestEditableCourseResource_SRCS ../../data/languages.qrc)
qt5_add_resources(TestEditableCourseResource_SRCS ../testdata/testdata.qrc)
add_executable(test_editablecourseresource ${TestEditableCourseResource_SRCS} )
target_link_libraries(test_editablecourseresource
    artikulatecore
    Qt5::Test
)
add_test(NAME test_editablecourseresource COMMAND test_editablecourseresource)
ecm_mark_as_test(test_editablecourseresource)


# test course model class
set(TestCourseModel_SRCS
    coursemodel/test_coursemodel.cpp
    ../mocks/resourcerepositorystub.cpp
    ../mocks/coursestub.cpp
    ../mocks/languagestub.cpp
)
qt5_add_resources(TestCourseModel_SRCS ../../data/languages.qrc)
qt5_add_resources(TestCourseModel_SRCS ../testdata/testdata.qrc)
add_executable(test_coursemodel ${TestCourseModel_SRCS} )
target_link_libraries(test_coursemodel
    artikulatecore
    Qt5::Test
)
add_test(NAME test_coursemodel COMMAND test_coursemodel)
ecm_mark_as_test(test_coursemodel)
