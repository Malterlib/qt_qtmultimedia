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

#ifndef AVFCAMERAFOCUSCONTROL_H
#define AVFCAMERAFOCUSCONTROL_H

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

#include <QtCore/qscopedpointer.h>
#include <QtCore/qglobal.h>

#include <private/qplatformcamerafocus_p.h>

#include <AVFoundation/AVFoundation.h>

@class AVCaptureDevice;

QT_BEGIN_NAMESPACE

class AVFCameraService;
class AVFCameraSession;

class AVFCameraFocusControl : public QPlatformCameraFocus
{
    Q_OBJECT
public:
    explicit AVFCameraFocusControl(AVFCameraService *service);

    QCameraFocus::FocusMode focusMode() const override;
    void setFocusMode(QCameraFocus::FocusMode mode) override;
    bool isFocusModeSupported(QCameraFocus::FocusMode mode) const override;

    QPointF focusPoint() const override;
    void setCustomFocusPoint(const QPointF &point) override;
    bool isCustomFocusPointSupported() const override;

    void setFocusDistance(float) override;
    float focusDistance() const override;

    ZoomRange zoomFactorRange() const override;
    void zoomTo(float newZoomFactor, float rate = -1.) override;

private Q_SLOTS:
    void cameraStateChanged();

private:
    AVFCameraSession *m_session;
    QCameraFocus::FocusMode m_focusMode;
    QPointF m_customFocusPoint;
    QPointF m_actualFocusPoint;

    CGFloat m_maxZoomFactor;
    CGFloat m_zoomFactor;
};


QT_END_NAMESPACE

#endif // AVFCAMERAFOCUSCONTROL_H
