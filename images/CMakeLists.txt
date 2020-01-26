# Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

FILE(GLOB images *.png)
install(FILES ${images} DESTINATION ${DATA_INSTALL_DIR}/artikulate/images)
