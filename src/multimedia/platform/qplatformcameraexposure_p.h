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

#ifndef QCAMERAEXPOSURECONTROL_H
#define QCAMERAEXPOSURECONTROL_H

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

#include <QtCore/qobject.h>
#include <QtMultimedia/qtmultimediaglobal.h>

#include <QtMultimedia/qcameraexposure.h>
#include <QtMultimedia/qcamera.h>
#include <QtMultimedia/qmediaenumdebug.h>

QT_BEGIN_NAMESPACE

// Required for QDoc workaround
class QString;

class Q_MULTIMEDIA_EXPORT QPlatformCameraExposure : public QObject
{
    Q_OBJECT
    Q_ENUMS(ExposureParameter)

public:
    enum ExposureParameter {
        ISO,
        Aperture,
        ShutterSpeed,
        ExposureCompensation,
        TorchPower,
        ExposureMode
    };

    virtual bool isParameterSupported(ExposureParameter parameter) const = 0;
    virtual QVariantList supportedParameterRange(ExposureParameter parameter, bool *continuous) const = 0;

    virtual QVariant requestedValue(ExposureParameter parameter) const = 0;
    virtual QVariant actualValue(ExposureParameter parameter) const = 0;
    virtual bool setValue(ExposureParameter parameter, const QVariant& value) = 0;

    virtual QCameraExposure::FlashMode flashMode() const = 0;
    virtual void setFlashMode(QCameraExposure::FlashMode mode) = 0;
    virtual bool isFlashModeSupported(QCameraExposure::FlashMode mode) const = 0;

    virtual QCameraExposure::TorchMode torchMode() const { return QCameraExposure::TorchOff; }
    virtual void setTorchMode(QCameraExposure::TorchMode /*mode*/) {}
    virtual bool isTorchModeSupported(QCameraExposure::TorchMode mode) const { return mode == QCameraExposure::TorchOff; }

    virtual bool isFlashReady() const = 0;

Q_SIGNALS:
    void flashReady(bool);
    void requestedValueChanged(int parameter);
    void actualValueChanged(int parameter);
    void parameterRangeChanged(int parameter);

protected:
    explicit QPlatformCameraExposure(QObject *parent = nullptr);
};

QT_END_NAMESPACE

Q_MEDIA_ENUM_DEBUG(QPlatformCameraExposure, ExposureParameter)


#endif  // QCAMERAEXPOSURECONTROL_H

