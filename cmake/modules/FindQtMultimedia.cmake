# - Find Qt Multimedia libs
#
# This module determines whether QtMultimedia (nowadays in QtMobility is installed.
#
# Copyright (c) 2013, Andreas Cord-Landwehr, <cordlandwehr@gmail.com>
#

# TODO add check wether not found by other module

FIND_PATH( QTMULTIMEDIA_INCLUDE_DIR NAMES qmediaobject.h PATH_SUFFIXES QtMultimediaKit )

FIND_PACKAGE_HANDLE_STANDARD_ARGS( QtMultimedia DEFAULT_MSG QTMULTIMEDIA_INCLUDE_DIR )

