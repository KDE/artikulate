# SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

function(add_language_resources TARGET)
    qt_add_resources(${TARGET} "languages"
        PREFIX "/artikulate/"
        BASE "${CMAKE_SOURCE_DIR}"
        FILES
            ${CMAKE_SOURCE_DIR}/data/languages/ba.xml
            ${CMAKE_SOURCE_DIR}/data/languages/ca.xml
            ${CMAKE_SOURCE_DIR}/data/languages/de.xml
            ${CMAKE_SOURCE_DIR}/data/languages/du.xml
            ${CMAKE_SOURCE_DIR}/data/languages/en_BE.xml
            ${CMAKE_SOURCE_DIR}/data/languages/en_US.xml
            ${CMAKE_SOURCE_DIR}/data/languages/fr.xml
            ${CMAKE_SOURCE_DIR}/data/languages/gr.xml
            ${CMAKE_SOURCE_DIR}/data/languages/hi.xml
            ${CMAKE_SOURCE_DIR}/data/languages/it.xml
            ${CMAKE_SOURCE_DIR}/data/languages/mr.xml
            ${CMAKE_SOURCE_DIR}/data/languages/sp.xml
            ${CMAKE_SOURCE_DIR}/data/languages/pl.xml
            ${CMAKE_SOURCE_DIR}/data/schemes/course.xsd
            ${CMAKE_SOURCE_DIR}/data/schemes/language.xsd
            ${CMAKE_SOURCE_DIR}/data/schemes/skeleton.xsd
    )
endfunction()

function(add_testdata_resources TARGET)
    qt_add_resources(${TARGET} "testdata"
        BASE "${CMAKE_SOURCE_DIR}/autotests/testdata/"
        FILES
            ${CMAKE_SOURCE_DIR}/autotests/testdata/courses/de_01.ogg
            ${CMAKE_SOURCE_DIR}/autotests/testdata/courses/de_02.ogg
            ${CMAKE_SOURCE_DIR}/autotests/testdata/courses/de.xml
            ${CMAKE_SOURCE_DIR}/autotests/testdata/courses/fr.xml
    )
endfunction()

add_subdirectory(unittests)
add_subdirectory(integrationtests)
add_subdirectory(resourcetests)

