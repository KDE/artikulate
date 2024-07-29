/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef ICONITEM_H
#define ICONITEM_H

#include <QIcon>
#include <QQuickItem>
#include <QVariant>

#include "artikulatecore_export.h"

class ARTIKULATECORE_EXPORT IconItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QVariant icon READ icon WRITE setIcon)
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth)
    Q_PROPERTY(int implicitWidth READ implicitWidth CONSTANT)
    Q_PROPERTY(int implicitHeight READ implicitHeight CONSTANT)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY stateChanged)

    Q_ENUMS(State)

public:
    enum State {
        DefaultState, /// The default state.
        ActiveState,  /// Icon is active.
        DisabledState /// Icon is disabled.
    };

    explicit IconItem(QQuickItem *parent = nullptr);
    ~IconItem() override;

    void setIcon(const QVariant &icon);
    QIcon icon() const;

    IconItem::State state() const;
    void setState(State state);

    int implicitWidth() const;
    int implicitHeight() const;

    void setSmooth(const bool smooth);
    bool smooth() const;

    void setEnabled(bool enabled = true);
    bool enabled() const;

    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *data) override;

Q_SIGNALS:
    void stateChanged(State state);

protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    QIcon m_icon;
    bool m_smooth;
    State m_state;
    bool m_changed;
};

#endif
