/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "languagestub.h"

// define one virtual method out of line to pin LanguageStub to this translation unit
LanguageStub::~LanguageStub() = default;
