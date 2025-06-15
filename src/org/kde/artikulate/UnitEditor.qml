// SPDX-FileCopyrightText: 2025 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.artikulate

ScrollView {
    id: root
    required property Unit unit

    ColumnLayout {
        Kirigami.FormLayout {
            TextField {
                id: i18nUnit
                Kirigami.FormData.label: i18n("Localized Unit:")
                text: root.unit.title
                onEditingFinished: {
                    root.unit.title = text
                }
            }
        }
    }
}
