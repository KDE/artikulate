# SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

set(learnerprofile_LIB_SRCS
    learner.cpp
    learninggoal.cpp
    profilemanager.cpp
    storage.cpp
    models/learninggoalmodel.cpp
    liblearner_debug.cpp
)

add_library(artikulatelearnerprofile SHARED ${learnerprofile_LIB_SRCS})
generate_export_header(artikulatelearnerprofile BASE_NAME liblearnerprofile)

target_link_libraries(
    artikulatelearnerprofile
    LINK_PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Sql
        Qt::Widgets
        KF6::ConfigCore
        KF6::I18n
)
# internal library without any API or ABI guarantee
set(GENERIC_LIB_VERSION "0")
set(GENERIC_LIB_SOVERSION "0")
set_target_properties(
    artikulatelearnerprofile
    PROPERTIES
        VERSION ${GENERIC_LIB_VERSION}
        SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS artikulatelearnerprofile
    LIBRARY NAMELINK_SKIP
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
