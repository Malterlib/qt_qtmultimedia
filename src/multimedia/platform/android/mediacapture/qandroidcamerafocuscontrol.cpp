/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include "qandroidcamerafocuscontrol_p.h"

#include "qandroidcamerasession_p.h"
#include "androidcamera_p.h"

#include "qandroidmultimediautils_p.h"
#include <qmath.h>

QT_BEGIN_NAMESPACE

static QRect adjustedArea(const QRectF &area)
{
    // Qt maps focus points in the range (0.0, 0.0) -> (1.0, 1.0)
    // Android maps focus points in the range (-1000, -1000) -> (1000, 1000)
    // Converts an area in Qt coordinates to Android coordinates
    return QRect(-1000 + qRound(area.x() * 2000),
                 -1000 + qRound(area.y() * 2000),
                 qRound(area.width() * 2000),
                 qRound(area.height() * 2000))
            .intersected(QRect(-1000, -1000, 2000, 2000));
}

QAndroidCameraFocusControl::QAndroidCameraFocusControl(QAndroidCameraSession *session)
    : QPlatformCameraFocus()
    , m_session(session)
    , m_focusMode(QCameraFocus::AutoFocus)
    , m_focusPoint(0.5, 0.5)
    , m_continuousPictureFocusSupported(false)
    , m_continuousVideoFocusSupported(false)
{
    connect(m_session, SIGNAL(opened()),
            this, SLOT(onCameraOpened()));
}

QCameraFocus::FocusMode QAndroidCameraFocusControl::focusMode() const
{
    return m_focusMode;
}

void QAndroidCameraFocusControl::setFocusMode(QCameraFocus::FocusMode mode)
{
    if (!m_session->camera()) {
        setFocusModeHelper(mode);
        return;
    }

    if (isFocusModeSupported(mode)) {
        QString focusMode;

        switch (mode) {
        case QCameraFocus::FocusModeHyperfocal:
            focusMode = QLatin1String("edof");
            break;
        case QCameraFocus::FocusModeInfinity: // not 100%, but close
            focusMode = QLatin1String("infinity");
            break;
        case QCameraFocus::FocusModeManual:
            focusMode = QLatin1String("fixed");
            break;
        case QCameraFocus::FocusModeAutoNear:
            focusMode = QLatin1String("macro");
            break;
        case QCameraFocus::FocusModeAuto:
        case QCameraFocus::FocusModeAutoFar:
            if (1) { // ###?
                focusMode = QLatin1String("continuous-video");
            } else {
                focusMode = QLatin1String("continuous-picture");
            }
            break;
        }

        m_session->camera()->setFocusMode(focusMode);

        // reset focus position
        m_session->camera()->cancelAutoFocus();

        setFocusModeHelper(mode);
    }
}

bool QAndroidCameraFocusControl::isFocusModeSupported(QCameraFocus::FocusMode mode) const
{
    return m_session->camera() ? m_supportedFocusModes.contains(mode) : false;
}

bool QAndroidCameraFocusControl::isCustomFocusPointSupported() const
{
    return m_focusPointSupported;
}

QPointF QAndroidCameraFocusControl::focusPoint() const
{
    return m_focusPoint;
}

void QAndroidCameraFocusControl::setCustomFocusPoint(const QPointF &point)
{
    if (m_focusPoint != point) {
        m_focusPoint = point;
        emit customFocusPointChanged(m_focusPoint);
    }

    if (m_session->camera())
        setCameraFocusArea();
}

void QAndroidCameraFocusControl::onCameraOpened()
{
    connect(m_session->camera(), SIGNAL(previewSizeChanged()),
            this, SLOT(onViewportSizeChanged()));
    connect(m_session->camera(), SIGNAL(autoFocusStarted()),
            this, SLOT(onAutoFocusStarted()));
    connect(m_session->camera(), SIGNAL(autoFocusComplete(bool)),
            this, SLOT(onAutoFocusComplete(bool)));

    m_supportedFocusModes.clear();
    m_continuousPictureFocusSupported = false;
    m_continuousVideoFocusSupported = false;
    m_focusPointSupported = false;

    QStringList focusModes = m_session->camera()->getSupportedFocusModes();
    for (int i = 0; i < focusModes.size(); ++i) {
        const QString &focusMode = focusModes.at(i);
        if (focusMode == QLatin1String("continuous-picture")) {
            m_supportedFocusModes << QCameraFocus::FocusModeAuto;
            m_continuousPictureFocusSupported = true;
        } else if (focusMode == QLatin1String("continuous-video")) {
            m_supportedFocusModes << QCameraFocus::FocusModeAuto;
            m_continuousVideoFocusSupported = true;
        } else if (focusMode == QLatin1String("edof")) {
            m_supportedFocusModes << QCameraFocus::FocusModeHyperfocal;
        } else if (focusMode == QLatin1String("fixed")) {
            m_supportedFocusModes << QCameraFocus::FocusModeManual;
        } else if (focusMode == QLatin1String("infinity")) {
            m_supportedFocusModes << QCameraFocus::FocusModeInfinity;
        } else if (focusMode == QLatin1String("macro")) {
            m_supportedFocusModes << QCameraFocus::FocusModeAutoNear;
        }
    }

    if (m_session->camera()->getMaxNumFocusAreas() > 0)
        m_focusPointSupported = true;

    if (!m_supportedFocusModes.contains(m_focusMode))
        setFocusModeHelper(QCameraFocus::AutoFocus);

    setFocusMode(m_focusMode);
    setCustomFocusPoint(m_focusPoint);

    if (m_session->camera()->isZoomSupported()) {
        m_zoomRatios = m_session->camera()->getZoomRatios();
        qreal maxZoom = m_zoomRatios.last() / qreal(100);
        if (m_maximumZoom != maxZoom) {
            m_maximumZoom = maxZoom;
        }
        zoomTo(1, -1);
    } else {
        m_zoomRatios.clear();
        m_maximumZoom = 1.0;
    }
}

void QAndroidCameraFocusControl::setCameraFocusArea()
{
    QList<QRect> areas;
    if (QRectF(0., 0., 1., 1.).contains(m_focusPoint)) {
        // in FocusPointAuto mode, leave the area list empty
        // to let the driver choose the focus point.
        QSize viewportSize = m_session->camera()->previewSize();

        if (!viewportSize.isValid())
            return;

        // Set up a 50x50 pixel focus area around the focal point
        QSizeF focusSize(50.f / viewportSize.width(), 50.f / viewportSize.height());
        float x = qBound(qreal(0),
                         m_focusPoint.x() - (focusSize.width() / 2),
                         1.f - focusSize.width());
        float y = qBound(qreal(0),
                         m_focusPoint.y() - (focusSize.height() / 2),
                         1.f - focusSize.height());

        QRectF area(QPointF(x, y), focusSize);

        areas.append(adjustedArea(area));
    }
    m_session->camera()->setFocusAreas(areas);
}

void QAndroidCameraFocusControl::onViewportSizeChanged()
{
    setCameraFocusArea();
}

void QAndroidCameraFocusControl::onCameraCaptureModeChanged()
{
//    if (m_session->camera() && m_focusMode == QCameraFocus::ContinuousFocus) {
//        QString focusMode;
//        if ((m_session->captureMode().testFlag(QCamera::CaptureVideo) && m_continuousVideoFocusSupported)
//                || !m_continuousPictureFocusSupported) {
//            focusMode = QLatin1String("continuous-video");
//        } else {
//            focusMode = QLatin1String("continuous-picture");
//        }
//        m_session->camera()->setFocusMode(focusMode);
//        m_session->camera()->cancelAutoFocus();
//    }
}

void QAndroidCameraFocusControl::onAutoFocusStarted()
{
}

void QAndroidCameraFocusControl::onAutoFocusComplete(bool /*success*/)
{
}


QAndroidCameraFocusControl::ZoomRange QAndroidCameraFocusControl::zoomFactorRange() const
{
    return { 1., m_maximumZoom };
}

void QAndroidCameraFocusControl::zoomTo(float factor, float rate)
{
    Q_UNUSED(rate);

    if (!qFuzzyCompare(m_requestedZoom, factor)) {
        m_requestedZoom = factor;
    }

    if (m_session->camera()) {
        factor = qBound(qreal(1), factor, m_maximumZoom);
        int validZoomIndex = qt_findClosestValue(m_zoomRatios, qRound(factor * 100));
        float newZoom = m_zoomRatios.at(validZoomIndex) / qreal(100);
        if (!qFuzzyCompare(m_currentZoom, newZoom)) {
            m_session->camera()->setZoom(validZoomIndex);
            m_currentZoom = newZoom;
        }
    }
}

QT_END_NAMESPACE
