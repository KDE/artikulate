/*
 *   Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MANAGEDTEXTURENODE_H
#define MANAGEDTEXTURENODE_H

#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QSharedPointer>

/**
 * @short Node that contains a reference counted texture
 *
 * Usually when assigning textures within a node, we'll want to delete the
 * texture with the node. This class will take a shared texture and display it
 * within the node.
 *
 * It's especially interesting to use this class together with the ImageTexturesCache
 * that will offer us shareable textures and cache them transparently, when asking
 * it to create the texture.
 *
 * @see ImageTexturesCache
 */

class ManagedTextureNode : public QSGSimpleTextureNode
{
    Q_DISABLE_COPY(ManagedTextureNode)
public:
    ManagedTextureNode();

    void setTexture(QSharedPointer<QSGTexture> texture);

private:
    QSharedPointer<QSGTexture> m_texture;
};

#endif
