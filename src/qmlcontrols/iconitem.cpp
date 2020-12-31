/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "iconitem.h"

#include "imagetexturescache.h"
#include "managedtexturenode.h"
#include <QIcon>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>

Q_GLOBAL_STATIC(ImageTexturesCache, s_iconImageCache)

IconItem::IconItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_smooth(false)
    , m_state(DefaultState)
    , m_changed(false)
{
    setFlag(ItemHasContents, true);
}

IconItem::~IconItem()
{
}

void IconItem::setIcon(const QVariant &icon)
{
    if (icon.canConvert<QIcon>()) {
        m_icon = icon.value<QIcon>();
    } else if (icon.canConvert<QString>()) {
        m_icon = QIcon::fromTheme(icon.toString());
    } else {
        m_icon = QIcon();
    }
    m_changed = true;
    update();
}

QIcon IconItem::icon() const
{
    return m_icon;
}

IconItem::State IconItem::state() const
{
    return m_state;
}

void IconItem::setState(IconItem::State state)
{
    if (m_state == state) {
        return;
    }

    m_state = state;
    m_changed = true;
    emit stateChanged(state);
    update();
}

bool IconItem::enabled() const
{
    return (m_state == DefaultState);
}

void IconItem::setEnabled(bool enabled)
{
    if (enabled) {
        setState(DefaultState);
    } else {
        setState(DisabledState);
    }
}

int IconItem::implicitWidth() const
{
    return 32;
}

int IconItem::implicitHeight() const
{
    return 32;
}

void IconItem::setSmooth(const bool smooth)
{
    if (smooth == m_smooth) {
        return;
    }
    m_smooth = smooth;
    m_changed = true;
    update();
}

bool IconItem::smooth() const
{
    return m_smooth;
}

QSGNode *IconItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData * /*data*/)
{
    if (m_icon.isNull()) {
        delete node;
        return nullptr;
    }

    if (m_changed || node == nullptr) {
        m_changed = false;

        ManagedTextureNode *mNode = dynamic_cast<ManagedTextureNode *>(node);
        if (!mNode) {
            delete node;
            mNode = new ManagedTextureNode;
        }

        QIcon::Mode mode;
        switch (m_state) {
            case DefaultState:
                mode = QIcon::Normal;
                break;
            case ActiveState:
                mode = QIcon::Active;
                break;
            case DisabledState:
                mode = QIcon::Disabled;
                break;
        }

        QImage img;
        const QSize size(static_cast<int>(width()), static_cast<int>(height()));
        if (!size.isEmpty()) {
            img = m_icon.pixmap(size, mode, QIcon::On).toImage();
        }
        mNode->setTexture(s_iconImageCache->loadTexture(window(), img));
        mNode->setRect(QRect(QPoint(0, 0), size));
        node = mNode;
    }

    return node;
}

void IconItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (newGeometry.size() != oldGeometry.size()) {
        m_changed = true;
        update();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}
