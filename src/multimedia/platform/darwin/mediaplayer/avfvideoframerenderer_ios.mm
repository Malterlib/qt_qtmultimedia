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

#include "private/avfvideoframerenderer_ios_p.h"

#include <QtMultimedia/qabstractvideosurface.h>
#include <QtOpenGL/QOpenGLFramebufferObject>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtGui/QOffscreenSurface>

#ifdef QT_DEBUG_AVF
#include <QtCore/qdebug.h>
#endif

#import <CoreVideo/CVBase.h>
#import <AVFoundation/AVFoundation.h>
QT_USE_NAMESPACE

AVFVideoFrameRenderer::AVFVideoFrameRenderer(QAbstractVideoSurface *surface, QObject *parent)
    : QObject(parent)
    , m_glContext(nullptr)
    , m_offscreenSurface(nullptr)
    , m_surface(surface)
    , m_textureCache(nullptr)
    , m_videoOutput(nullptr)
    , m_isContextShared(true)
{
}

AVFVideoFrameRenderer::~AVFVideoFrameRenderer()
{
#ifdef QT_DEBUG_AVF
    qDebug() << Q_FUNC_INFO;
#endif

    [m_videoOutput release]; // sending to nil is fine
    if (m_textureCache)
        CFRelease(m_textureCache);
    if (m_metalTextureCache)
        CFRelease(m_metalTextureCache);
    [m_metalDevice release];
    delete m_offscreenSurface;
    delete m_glContext;
}

void AVFVideoFrameRenderer::setPlayerLayer(AVPlayerLayer *layer)
{
    Q_UNUSED(layer);
    if (m_videoOutput) {
        [m_videoOutput release];
        m_videoOutput = nullptr;
        // will be re-created in first call to copyPixelBufferFromLayer
    }
}

quint64 AVFVideoFrameRenderer::renderLayerToMTLTexture(AVPlayerLayer *layer)
{
    if (!m_metalDevice)
        m_metalDevice = MTLCreateSystemDefaultDevice();

    if (!m_metalTextureCache) {
        CVReturn err = CVMetalTextureCacheCreate(kCFAllocatorDefault, nullptr,
            m_metalDevice, nullptr, &m_metalTextureCache);
        if (err) {
            qWarning() << "Error at CVMetalTextureCacheCreate" << err;
            return 0;
        }
    }

    size_t width = 0, height = 0;
    CVPixelBufferRef pixelBuffer = copyPixelBufferFromLayer(layer, width, height);

    if (!pixelBuffer)
        return 0;

    CVMetalTextureCacheFlush(m_metalTextureCache, 0);

    CVMetalTextureRef texture = nil;
    CVReturn err = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, m_metalTextureCache, pixelBuffer, NULL,
                                                             MTLPixelFormatBGRA8Unorm_sRGB, width, height, 0, &texture);

    if (!texture || err)
        qWarning("CVMetalTextureCacheCreateTextureFromImage failed (error: %d)", err);

    CVPixelBufferRelease(pixelBuffer);
    quint64 tex = 0;
    if (texture)  {
        tex = quint64(CVMetalTextureGetTexture(texture));
        CFRelease(texture);
    }

    return tex;
}

quint64 AVFVideoFrameRenderer::renderLayerToTexture(AVPlayerLayer *layer)
{
    initRenderer();

    // If the glContext isn't shared, it doesn't make sense to return a texture for us
    if (!m_isContextShared)
        return 0;

    size_t dummyWidth = 0, dummyHeight = 0;
    auto texture = createCacheTextureFromLayer(layer, dummyWidth, dummyHeight);
    auto tex = quint64(CVOGLTextureGetName(texture));
    CFRelease(texture);

    return tex;
}

static NSString* const AVF_PIXEL_FORMAT_KEY = (NSString*)kCVPixelBufferPixelFormatTypeKey;
static NSNumber* const AVF_PIXEL_FORMAT_VALUE = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
static NSDictionary* const AVF_OUTPUT_SETTINGS = [NSDictionary dictionaryWithObject:AVF_PIXEL_FORMAT_VALUE forKey:AVF_PIXEL_FORMAT_KEY];


CVPixelBufferRef AVFVideoFrameRenderer::copyPixelBufferFromLayer(AVPlayerLayer *layer,
    size_t& width, size_t& height)
{
    //Is layer valid
    if (!layer) {
#ifdef QT_DEBUG_AVF
        qWarning("copyPixelBufferFromLayer: invalid layer");
#endif
        return nullptr;
    }

    if (!m_videoOutput) {
        m_videoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:AVF_OUTPUT_SETTINGS];
        [m_videoOutput setDelegate:nil queue:nil];
        AVPlayerItem * item = [[layer player] currentItem];
        [item addOutput:m_videoOutput];
    }

    CFTimeInterval currentCAFrameTime =  CACurrentMediaTime();
    CMTime currentCMFrameTime =  [m_videoOutput itemTimeForHostTime:currentCAFrameTime];
    // happens when buffering / loading
    if (CMTimeCompare(currentCMFrameTime, kCMTimeZero) < 0) {
        return nullptr;
    }

    CVPixelBufferRef pixelBuffer = [m_videoOutput copyPixelBufferForItemTime:currentCMFrameTime
                                                   itemTimeForDisplay:nil];
    if (!pixelBuffer) {
#ifdef QT_DEBUG_AVF
        qWarning("copyPixelBufferForItemTime returned nil");
        CMTimeShow(currentCMFrameTime);
#endif
        return nullptr;
    }

    width = CVPixelBufferGetWidth(pixelBuffer);
    height = CVPixelBufferGetHeight(pixelBuffer);
    return pixelBuffer;
}

CVOGLTextureRef AVFVideoFrameRenderer::createCacheTextureFromLayer(AVPlayerLayer *layer,
        size_t& width, size_t& height)
{
    CVPixelBufferRef pixelBuffer = copyPixelBufferFromLayer(layer, width, height);

    if (!pixelBuffer)
        return nullptr;

    CVOGLTextureCacheFlush(m_textureCache, 0);

    CVOGLTextureRef texture = nullptr;
    CVReturn err = CVOGLTextureCacheCreateTextureFromImage(kCFAllocatorDefault, m_textureCache, pixelBuffer, nullptr,
                                                           GL_TEXTURE_2D, GL_RGBA,
                                                           (GLsizei) width, (GLsizei) height,
                                                           GL_BGRA, GL_UNSIGNED_BYTE, 0,
                                                           &texture);

    if (!texture || err) {
#ifdef QT_DEBUG_AVF
        qWarning("CVOGLTextureCacheCreateTextureFromImage failed (error: %d)", err);
#endif
    }

    CVPixelBufferRelease(pixelBuffer);

    return texture;
}

QImage AVFVideoFrameRenderer::renderLayerToImage(AVPlayerLayer *layer)
{
    size_t width = 0;
    size_t height = 0;
    CVPixelBufferRef pixelBuffer = copyPixelBufferFromLayer(layer, width, height);

    if (!pixelBuffer)
        return QImage();

    OSType pixelFormat = CVPixelBufferGetPixelFormatType(pixelBuffer);
    if (pixelFormat != kCVPixelFormatType_32BGRA) {
#ifdef QT_DEBUG_AVF
        qWarning("CVPixelBuffer format is not BGRA32 (got: %d)", static_cast<quint32>(pixelFormat));
#endif
        return QImage();
    }

    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    char *data = (char *)CVPixelBufferGetBaseAddress(pixelBuffer);
    size_t stride = CVPixelBufferGetBytesPerRow(pixelBuffer);

    // format here is not relevant, only using for storage
    QImage img = QImage(width, height, QImage::Format_ARGB32);
    for (size_t j = 0; j < height; j++) {
        memcpy(img.scanLine(j), data, width * 4);
        data += stride;
    }

    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    CVPixelBufferRelease(pixelBuffer);
    return img;
}

void AVFVideoFrameRenderer::initRenderer()
{
    // even for using a texture directly, we need to be able to make a context current,
    // so we need an offscreen, and we shouldn't assume we can make the surface context
    // current on that offscreen, so use our own (sharing with it). Slightly
    // excessive but no performance penalty and makes the QImage path easier to maintain

    //Make sure we have an OpenGL context to make current
    if (!m_glContext) {
        //Create OpenGL context and set share context from surface
        QOpenGLContext *shareContext = nullptr;
        if (m_surface) {
            shareContext = qobject_cast<QOpenGLContext*>(m_surface->property("GLContext").value<QObject*>());
        }

        m_glContext = new QOpenGLContext();
        if (shareContext) {
            m_glContext->setShareContext(shareContext);
            m_isContextShared = true;
        } else {
#ifdef QT_DEBUG_AVF
            qWarning("failed to get Render Thread context");
#endif
            m_isContextShared = false;
        }
        if (!m_glContext->create()) {
#ifdef QT_DEBUG_AVF
            qWarning("failed to create QOpenGLContext");
#endif
            return;
        }
    }

    if (!m_offscreenSurface) {
        m_offscreenSurface = new QOffscreenSurface();
        m_offscreenSurface->setFormat(m_glContext->format());
        m_offscreenSurface->create();
    }

    //Need current context
    m_glContext->makeCurrent(m_offscreenSurface);

    if (!m_textureCache) {
        //  Create a new open gl texture cache
        CVReturn err = CVOGLTextureCacheCreate(kCFAllocatorDefault, nullptr,
            [EAGLContext currentContext],
            nullptr, &m_textureCache);
        if (err) {
    #ifdef QT_DEBUG_AVF
            qWarning("Error at CVOGLTextureCacheCreate %d", err);
    #endif
        }
    }

}
