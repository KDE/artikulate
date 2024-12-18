// SPDX-FileCopyrightText: 2018-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#ifndef DRAWERTRAININGACTIONS_H
#define DRAWERTRAININGACTIONS_H

#include <KDescendantsProxyModel>
#include <QObject>
#include <QQmlEngine>

class DrawerTrainingActions : public KDescendantsProxyModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles {
        Action = Qt::UserRole + 1,
    };
    Q_ENUM(Roles)

    explicit DrawerTrainingActions(QObject *parent = nullptr);
    /**
     * @copydoc QAbstractItemModel::rolesNames()
     */
    QHash<int, QByteArray> roleNames() const override;

    void setSourceModel(QAbstractItemModel *model) override;

Q_SIGNALS:
    /**
     * Notify that course view shall be displayed.
     */
    void triggerPhraseView();
    void currentIndexChanged(int row);

public Q_SLOTS:
    void trigger(int index);
};

#endif
