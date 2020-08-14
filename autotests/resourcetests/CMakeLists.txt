# SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

include_directories(
    ../../src/
    ../../
    ../mocks/
    ${CMAKE_CURRENT_BINARY_DIR}
)

# basic tests language resource conformance with XSD
set(TestLanguageFiles_SRCS
    languageresources/testlanguagefiles.cpp
)
qt5_add_resources(TestLanguageFiles_SRCS ../../data/languages.qrc)
add_executable(test_languageresources ${TestLanguageFiles_SRCS} )
target_link_libraries(test_languageresources
    artikulatecore
    Qt5::Test
)
add_test(NAME test_languageresources COMMAND test_languageresources)
ecm_mark_as_test(test_languageresources)


# test correctness of XSD files
set(TestXsdSchemes_SRCS
    xsdschemes/testxsdschemes.cpp
)
qt5_add_resources(TestXsdSchemes_SRCS ../../data/languages.qrc)
add_executable(test_xsdschemes ${TestXsdSchemes_SRCS} )
target_link_libraries(test_xsdschemes
    Qt5::Test
    Qt5::XmlPatterns
    Qt5::Core
)
add_test(NAME test_xsdschemes COMMAND test_xsdschemes)
ecm_mark_as_test(test_xsdschemes)
