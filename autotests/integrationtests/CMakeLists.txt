# SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(
    ../../src/
    ../../
    ${CMAKE_CURRENT_BINARY_DIR}
)

# copy test data
file(COPY ../testdata/courses/de.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/courses/de/) # copy test files
file(COPY ../testdata/courses/fr.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/courses/fr/) # copy test files
file(COPY ../testdata/contributorrepository/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/contributorrepository/) # copy test files

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# integration tests for iresource repository interface derived classes
add_executable(test_iresourcerepository_integration)
target_sources(test_iresourcerepository_integration PRIVATE
    iresourcerepository_integration/test_iresourcerepository.cpp
    iresourcerepository_integration/test_iresourcerepository.h
)
add_language_resources(test_iresourcerepository_integration)
target_link_libraries(test_iresourcerepository_integration
    artikulatecore
    Qt::Test
)
add_test(NAME test_iresourcerepository_integration COMMAND test_iresourcerepository_integration)
ecm_mark_as_test(test_iresourcerepository_integration)
