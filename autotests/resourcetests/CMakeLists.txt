# SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(
    ../../src/
    ../../
    ../mocks/
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src/
)

# basic tests language resource conformance with XSD
add_executable(test_languageresources)
target_sources(test_languageresources PRIVATE
    languageresources/testlanguagefiles.cpp
    languageresources/testlanguagefiles.h
)
add_language_resources(test_languageresources)
target_link_libraries(test_languageresources
    artikulatecore
    Qt::Test
)
add_test(NAME test_languageresources COMMAND test_languageresources)
ecm_mark_as_test(test_languageresources)


# test correctness of XSD files
add_executable(test_xsdschemes)
target_sources(test_xsdschemes PRIVATE
    xsdschemes/testxsdschemes.cpp
    xsdschemes/testxsdschemes.h
)
add_language_resources(test_xsdschemes)
target_link_libraries(test_xsdschemes
    artikulatecore
    Qt::Test
    Qt::Core
)
add_test(NAME test_xsdschemes COMMAND test_xsdschemes)
ecm_mark_as_test(test_xsdschemes)
