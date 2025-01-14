/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QDECLARATIVECAMERAFOCUS_H
#define QDECLARATIVECAMERAFOCUS_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qabstractitemmodel.h>
#include <qcamera.h>
#include <qcamerafocus.h>
#include "qdeclarativecamera_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeCamera;

class QDeclarativeCameraFocus : public QObject
{
    Q_OBJECT

    Q_PROPERTY(FocusMode focusMode READ focusMode WRITE setFocusMode NOTIFY focusModeChanged)
    Q_PROPERTY(QVariantList supportedFocusMode READ supportedFocusMode NOTIFY supportedFocusModeChanged REVISION 1)

    Q_PROPERTY(QPointF customFocusPoint READ customFocusPoint WRITE setCustomFocusPoint NOTIFY customFocusPointChanged)

    Q_ENUMS(FocusMode)
public:
    enum FocusMode {
        FocusModeAuto = QCameraFocus::FocusModeAuto,
        FocusModeAutoNear = QCameraFocus::FocusModeAutoNear,
        FocusModeAutoFar = QCameraFocus::FocusModeAutoFar,
        FocusModeHyperfocal = QCameraFocus::FocusModeHyperfocal,
        FocusModeInfinity = QCameraFocus::FocusModeInfinity,
        FocusModeManual = QCameraFocus::FocusModeManual
#if 1 // QT_DEPRECATED
        , FocusContinuous = FocusModeAuto,
        FocusAuto = FocusModeAuto,
        FocusMacro = FocusModeAutoNear,
        FocusHyperfocal = FocusModeHyperfocal,
        FocusInfinity = FocusModeInfinity,
        FocusManual = FocusModeManual
#endif
    };

    ~QDeclarativeCameraFocus();

    FocusMode focusMode() const;
    QVariantList supportedFocusMode() const;

    QPointF customFocusPoint() const;

public Q_SLOTS:
    void setFocusMode(FocusMode);
    void setCustomFocusPoint(const QPointF &point);

Q_SIGNALS:
    void focusModeChanged(FocusMode);
    void supportedFocusModeChanged();
    void customFocusPointChanged(const QPointF &);

private:
    friend class QDeclarativeCamera;
    QDeclarativeCameraFocus(QCamera *camera, QObject *parent = 0);

    QCameraFocus *m_focus;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeCameraFocus))

#endif
