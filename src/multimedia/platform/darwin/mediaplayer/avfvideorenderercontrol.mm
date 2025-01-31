/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "avfvideorenderercontrol_p.h"
#include "avfdisplaylink_p.h"

#if defined(Q_OS_IOS) || defined(Q_OS_TVOS)
#include "avfvideoframerenderer_ios_p.h"
#else
#include "avfvideoframerenderer_p.h"
#endif

#include <private/qabstractvideobuffer_p.h>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

#include <private/qimagevideobuffer_p.h>

#include <QtCore/qdebug.h>

#import <AVFoundation/AVFoundation.h>

QT_USE_NAMESPACE

class TextureVideoBuffer : public QAbstractVideoBuffer
{
public:
    TextureVideoBuffer(QVideoFrame::HandleType type, quint64 tex)
        : QAbstractVideoBuffer(type)
        , m_texture(tex)
    {}

    virtual ~TextureVideoBuffer()
    {
    }

    QVideoFrame::MapMode mapMode() const override { return QVideoFrame::NotMapped; }
    MapData map(QVideoFrame::MapMode /*mode*/) override { return {}; }
    void unmap() override {}

    QVariant handle() const override
    {
        return QVariant::fromValue<unsigned long long>(m_texture);
    }

private:
    quint64 m_texture;
};

AVFVideoRendererControl::AVFVideoRendererControl(QObject *parent)
    : QObject(parent)
    , m_surface(nullptr)
    , m_playerLayer(nullptr)
    , m_frameRenderer(nullptr)
    , m_enableOpenGL(false)
    , m_enableMetal(false)

{
    m_displayLink = new AVFDisplayLink(this);
    connect(m_displayLink, SIGNAL(tick(CVTimeStamp)), SLOT(updateVideoFrame(CVTimeStamp)));
}

AVFVideoRendererControl::~AVFVideoRendererControl()
{
#ifdef QT_DEBUG_AVF
    qDebug() << Q_FUNC_INFO;
#endif
    m_displayLink->stop();
    [static_cast<AVPlayerLayer*>(m_playerLayer) release];
}

QAbstractVideoSurface *AVFVideoRendererControl::surface() const
{
    return m_surface;
}

void AVFVideoRendererControl::setSurface(QAbstractVideoSurface *surface)
{
#ifdef QT_DEBUG_AVF
    qDebug() << "Set video surface" << surface;
#endif

    //When we have a valid surface, we can setup a frame renderer
    //and schedule surface updates with the display link.
    if (surface == m_surface)
        return;

    QMutexLocker locker(&m_mutex);

    if (m_surface && m_surface->isActive())
        m_surface->stop();

    m_surface = surface;

    //If the surface changed, then the current frame renderer is no longer valid
    delete m_frameRenderer;
    m_frameRenderer = nullptr;

    //If there is now no surface to render too
    if (m_surface == nullptr) {
        m_displayLink->stop();
        return;
    }

    //Surface changed, so we need a new frame renderer
    m_frameRenderer = new AVFVideoFrameRenderer(m_surface, this);
#if defined(Q_OS_IOS) || defined(Q_OS_TVOS)
    if (m_playerLayer) {
        m_frameRenderer->setPlayerLayer(static_cast<AVPlayerLayer*>(m_playerLayer));
    }
#endif

    auto checkHandleType = [this] {
        m_enableOpenGL = m_surface->supportedPixelFormats(QVideoFrame::GLTextureHandle).contains(QVideoFrame::Format_BGR32);
        m_enableMetal = m_surface->supportedPixelFormats(QVideoFrame::MTLTextureHandle).contains(QVideoFrame::Format_BGR32);
    };
    checkHandleType();
    connect(m_surface, &QAbstractVideoSurface::supportedFormatsChanged, this, checkHandleType);

    //If we already have a layer, but changed surfaces start rendering again
    if (m_playerLayer && !m_displayLink->isActive()) {
        m_displayLink->start();
    }

}

void AVFVideoRendererControl::setLayer(CALayer *playerLayer)
{
    if (m_playerLayer == playerLayer)
        return;

    m_playerLayer = [static_cast<AVPlayerLayer*>(playerLayer) retain];

    //If there is an active surface, make sure it has been stopped so that
    //we can update it's state with the new content.
    if (m_surface && m_surface->isActive())
        m_surface->stop();

#if defined(Q_OS_IOS) || defined(Q_OS_TVOS)
    if (m_frameRenderer) {
        m_frameRenderer->setPlayerLayer(static_cast<AVPlayerLayer*>(playerLayer));
    }
#endif

    //If there is no layer to render, stop scheduling updates
    if (m_playerLayer == nullptr) {
        m_displayLink->stop();
        return;
    }

    setupVideoOutput();

    //If we now have both a valid surface and layer, start scheduling updates
    if (m_surface && !m_displayLink->isActive()) {
        m_displayLink->start();
    }
}

void AVFVideoRendererControl::updateVideoFrame(const CVTimeStamp &ts)
{
    Q_UNUSED(ts);

    AVPlayerLayer *playerLayer = static_cast<AVPlayerLayer*>(m_playerLayer);

    if (!playerLayer) {
        qWarning("updateVideoFrame called without AVPlayerLayer (which shouldn't happen");
        return;
    }

    if (!playerLayer.readyForDisplay || !m_surface)
        return;

    if (m_enableMetal) {
        quint64 tex = m_frameRenderer->renderLayerToMTLTexture(playerLayer);
        if (tex == 0)
            return;

        auto buffer = new TextureVideoBuffer(QVideoFrame::MTLTextureHandle, tex);
        QVideoFrame frame(buffer, m_nativeSize, QVideoFrame::Format_BGR32);
        if (m_surface->isActive() && m_surface->surfaceFormat().pixelFormat() != frame.pixelFormat())
            m_surface->stop();

        if (!m_surface->isActive()) {
            QVideoSurfaceFormat format(frame.size(), frame.pixelFormat(), QVideoFrame::MTLTextureHandle);
#if defined(Q_OS_IOS) || defined(Q_OS_TVOS)
            format.setScanLineDirection(QVideoSurfaceFormat::TopToBottom);
#else
            format.setScanLineDirection(QVideoSurfaceFormat::BottomToTop);
#endif
            if (!m_surface->start(format))
                qWarning("Failed to activate video surface");
        }

        if (m_surface->isActive())
            m_surface->present(frame);

        return;
    }

    if (m_enableOpenGL) {
        quint64 tex = m_frameRenderer->renderLayerToTexture(playerLayer);
        //Make sure we got a valid texture
        if (tex == 0)
            return;

        QAbstractVideoBuffer *buffer = new TextureVideoBuffer(QVideoFrame::GLTextureHandle, tex);
        QVideoFrame frame = QVideoFrame(buffer, m_nativeSize, QVideoFrame::Format_BGR32);

        if (m_surface && frame.isValid()) {
            if (m_surface->isActive() && m_surface->surfaceFormat().pixelFormat() != frame.pixelFormat())
                m_surface->stop();

            if (!m_surface->isActive()) {
                QVideoSurfaceFormat format(frame.size(), frame.pixelFormat(), QVideoFrame::GLTextureHandle);
#if defined(Q_OS_IOS) || defined(Q_OS_TVOS)
                format.setScanLineDirection(QVideoSurfaceFormat::TopToBottom);
#else
                format.setScanLineDirection(QVideoSurfaceFormat::BottomToTop);
#endif
                if (!m_surface->start(format)) {
                    //Surface doesn't support GLTextureHandle
                    qWarning("Failed to activate video surface");
                }
            }

            if (m_surface->isActive())
                m_surface->present(frame);
        }
    } else {
        //fallback to rendering frames to QImages
        QImage frameData = m_frameRenderer->renderLayerToImage(playerLayer);

        if (frameData.isNull()) {
            return;
        }

        QAbstractVideoBuffer *buffer = new QImageVideoBuffer(frameData);
        QVideoFrame frame = QVideoFrame(buffer, m_nativeSize, QVideoFrame::Format_ARGB32);
        if (m_surface && frame.isValid()) {
            if (m_surface->isActive() && m_surface->surfaceFormat().pixelFormat() != frame.pixelFormat())
                m_surface->stop();

            if (!m_surface->isActive()) {
                QVideoSurfaceFormat format(frame.size(), frame.pixelFormat(), QVideoFrame::NoHandle);

                if (!m_surface->start(format)) {
                    qWarning("Failed to activate video surface");
                }
            }

            if (m_surface->isActive())
                m_surface->present(frame);
        }

    }
}

void AVFVideoRendererControl::setupVideoOutput()
{
    AVPlayerLayer *playerLayer = static_cast<AVPlayerLayer*>(m_playerLayer);
    if (playerLayer)
        m_nativeSize = QSize(playerLayer.bounds.size.width, playerLayer.bounds.size.height);
}


#include "moc_avfvideorenderercontrol_p.cpp"
