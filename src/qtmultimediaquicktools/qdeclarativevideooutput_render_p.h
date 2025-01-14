/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2016 Research In Motion
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

#ifndef QDECLARATIVEVIDEOOUTPUT_RENDER_P_H
#define QDECLARATIVEVIDEOOUTPUT_RENDER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQuick/qquickitem.h>
#include <QtQuick/qsgnode.h>
#include <private/qsgvideonode_yuv_p.h>
#include <private/qsgvideonode_rgb_p.h>
#include <private/qsgvideonode_texture_p.h>

#include <QtCore/qmutex.h>
#include <QtMultimedia/qabstractvideosurface.h>

QT_BEGIN_NAMESPACE

class QSGVideoItemSurface;
class QObject;
class QAbstractVideoFilter;
class QVideoFilterRunnable;
class QDeclarativeVideoOutput;

class QDeclarativeVideoBackend
{
public:
    QDeclarativeVideoBackend(QDeclarativeVideoOutput *parent);
    ~QDeclarativeVideoBackend();

    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &changeData);
    QSize nativeSize() const;
    void updateGeometry();
    QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data);
    QAbstractVideoSurface *videoSurface() const;
    QRectF adjustedViewport() const;

    friend class QSGVideoItemSurface;
    void present(const QVideoFrame &frame);
    void stop();

    void appendFilter(QAbstractVideoFilter *filter);
    void clearFilters();
    void releaseResources();
    void invalidateSceneGraph();

private:
    void scheduleDeleteFilterResources();
    QDeclarativeVideoOutput *q;

    QList<QSGVideoNodeFactoryInterface*> m_videoNodeFactories;
    QSGVideoItemSurface *m_surface;
    QVideoSurfaceFormat m_surfaceFormat;

    QVideoFrame m_frame;
    QVideoFrame m_frameOnFlush;
    bool m_frameChanged;
    QSGVideoNodeFactory_YUV m_i420Factory;
    QSGVideoNodeFactory_RGB m_rgbFactory;
    QSGVideoNodeFactory_Texture m_textureFactory;
    QMutex m_frameMutex;
    QRectF m_renderedRect;         // Destination pixel coordinates, clipped
    QRectF m_sourceTextureRect;    // Source texture coordinates

    struct Filter {
        Filter() : filter(0), runnable(0) { }
        Filter(QAbstractVideoFilter *filter) : filter(filter), runnable(0) { }
        QAbstractVideoFilter *filter;
        QVideoFilterRunnable *runnable;
    };
    QList<Filter> m_filters;
};

class QSGVideoItemSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit QSGVideoItemSurface(QDeclarativeVideoBackend *backend, QObject *parent = 0);
    ~QSGVideoItemSurface();
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QVideoFrame::HandleType handleType) const override;
    bool start(const QVideoSurfaceFormat &format) override;
    void stop() override;
    bool present(const QVideoFrame &frame) override;

private:
    QDeclarativeVideoBackend *m_backend;
};


namespace {

inline bool qIsDefaultAspect(int o)
{
    return (o % 180) == 0;
}

/*
 * Return the orientation normalized to 0-359
 */
inline int qNormalizedOrientation(int o)
{
    // Negative orientations give negative results
    int o2 = o % 360;
    if (o2 < 0)
        o2 += 360;
    return o2;
}

}

QT_END_NAMESPACE

#endif
