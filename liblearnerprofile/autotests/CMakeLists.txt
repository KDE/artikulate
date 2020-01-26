# Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(
    ../src/
    ../
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(TestLearnerStorage_SRCS
    testlearnerstorage.cpp
    ../src/storage.cpp
    ../src/liblearner_debug.cpp
)
add_executable(TestLearnerStorage ${TestLearnerStorage_SRCS} )
target_link_libraries(TestLearnerStorage
    artikulatelearnerprofile
    Qt5::Test
)
add_test(NAME TestLearnerStorage COMMAND TestLearnerStorage)
ecm_mark_as_test(TestLearnerStorage)
