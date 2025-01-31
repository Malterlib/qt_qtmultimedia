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

#include "qplatformcameraexposure_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QPlatformCameraExposure
    \obsolete

    \brief The QPlatformCameraExposure class allows controlling camera exposure parameters.

    \inmodule QtMultimedia

    \ingroup multimedia_control

    The QCameraExposure class is the usual method of adjusting exposure related parameters
    when using camera functionality.  This class provides a more complete but less easy
    to use interface, and also forms the interface to implement when writing a new
    implementation of QCamera functionality.

    You can adjust a number of parameters that will affect images and video taken with
    the corresponding QCamera object - see the \l {QPlatformCameraExposure::ExposureParameter}{ExposureParameter} enumeration.

    \sa QCameraExposure, QCamera
*/

/*!
    Constructs a camera exposure control object with \a parent.
*/
QPlatformCameraExposure::QPlatformCameraExposure(QObject *parent)
    : QObject(parent)
{
}

/*!
  \enum QPlatformCameraExposure::ExposureParameter
  \value ISO
         Camera ISO sensitivity, specified as integer value.
  \value Aperture
         Lens aperture is specified as an qreal F number.
         The supported apertures list can change depending on the focal length,
         in such a case the exposureParameterRangeChanged() signal is emitted.
  \value ShutterSpeed
         Shutter speed in seconds, specified as qreal.
  \value ExposureCompensation
         Exposure compensation, specified as qreal EV value.
  \value TorchPower
         Manual torch power, specified as qreal value.
         Accepted power range is [0..1.0],
         with 0 value means no light and 1.0 corresponds to full torch power.

         This value is only used in the \l{QCameraExposure::FlashVideolight}{torch flash mode}.
  \value ExposureMode
         Camera exposure mode.
*/

/*!
  \fn QPlatformCameraExposure::isParameterSupported(ExposureParameter parameter) const

  Returns true is exposure \a parameter is supported by backend.
  \since 5.0
*/

/*!
  \fn QPlatformCameraExposure::requestedValue(ExposureParameter parameter) const

  Returns the requested exposure \a parameter value.

  \since 5.0
*/

/*!
  \fn QPlatformCameraExposure::actualValue(ExposureParameter parameter) const

  Returns the actual exposure \a parameter value, or invalid QVariant() if the value is unknown or not supported.

  The actual parameter value may differ for the requested one if automatic mode is selected or
  camera supports only limited set of values within the supported range.
  \since 5.0
*/


/*!
  \fn QPlatformCameraExposure::supportedParameterRange(ExposureParameter parameter, bool *continuous = 0) const

  Returns the list of supported \a parameter values;

  If the camera supports arbitrary exposure parameter value within the supported range,
  *\a continuous is set to true, otherwise *\a continuous is set to false.

  \since 5.0
*/

/*!
  \fn bool QPlatformCameraExposure::setValue(ExposureParameter parameter, const QVariant& value)

  Set the exposure \a parameter to \a value.
  If a null or invalid QVariant is passed, backend should choose the value automatically,
  and if possible report the actual value to user with QPlatformCameraExposure::actualValue().

  Returns true if parameter is supported and value is correct.
  \since 5.0
*/

/*!
    \fn void QPlatformCameraExposure::requestedValueChanged(int parameter)

    Signal emitted when the requested exposure \a parameter value has changed,
    usually in result of setValue() call.
    \since 5.0
*/

/*!
    \fn void QPlatformCameraExposure::actualValueChanged(int parameter)

    Signal emitted when the actual exposure \a parameter value has changed,
    usually in result of auto exposure algorithms or manual exposure parameter applied.

    \since 5.0
*/

/*!
    \fn void QPlatformCameraExposure::parameterRangeChanged(int parameter)

    Signal emitted when the supported range of exposure \a parameter values has changed.

    \since 5.0
*/

QT_END_NAMESPACE

#include "moc_qplatformcameraexposure_p.cpp"
