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
add_executable(test_xsdschemes ${TestXsdSchemes_SRCS} )
target_link_libraries(test_xsdschemes
    Qt5::Test
    Qt5::XmlPatterns
    Qt5::Core
)
add_test(NAME test_xsdschemes COMMAND test_xsdschemes)
ecm_mark_as_test(test_xsdschemes)
