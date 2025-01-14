# Generated from multimedia.pro.

#####################################################################
## Multimedia Module:
#####################################################################

qt_internal_find_apple_system_framework(FWCoreMedia CoreMedia) # special case
qt_internal_find_apple_system_framework(FWCoreAudio CoreAudio) # special case
qt_internal_find_apple_system_framework(FWAudioUnit AudioUnit) # special case
qt_internal_find_apple_system_framework(FWAVFoundation AVFoundation) # special case

qt_internal_add_module(Multimedia
    PLUGIN_TYPES video/gstvideorenderer video/videonode
    SOURCES
        audio/qaudio.cpp audio/qaudio.h
        audio/qaudiobuffer.cpp audio/qaudiobuffer.h audio/qaudiobuffer_p.h
        audio/qaudiodecoder.cpp audio/qaudiodecoder.h
        audio/qaudiodeviceinfo.cpp audio/qaudiodeviceinfo.h audio/qaudiodeviceinfo_p.h
        audio/qaudioformat.cpp audio/qaudioformat.h
        audio/qaudiohelpers.cpp audio/qaudiohelpers_p.h
        audio/qaudioinput.cpp audio/qaudioinput.h
        audio/qaudiooutput.cpp audio/qaudiooutput.h
        audio/qaudiosystem.cpp audio/qaudiosystem_p.h
        audio/qsamplecache_p.cpp audio/qsamplecache_p.h
        audio/qsoundeffect.cpp audio/qsoundeffect.h
        audio/qwavedecoder.cpp audio/qwavedecoder.h
        camera/qcamera.cpp camera/qcamera.h camera/qcamera_p.h
        camera/qcameraexposure.cpp camera/qcameraexposure.h
        camera/qcamerafocus.cpp camera/qcamerafocus.h
        camera/qcameraimagecapture.cpp camera/qcameraimagecapture.h
        camera/qcameraimageprocessing.cpp camera/qcameraimageprocessing.h
        camera/qcamerainfo.cpp camera/qcamerainfo.h camera/qcamerainfo_p.h
        controls/qvideowindowcontrol.cpp controls/qvideowindowcontrol.h
        platform/qplatformaudiodecoder.cpp platform/qplatformaudiodecoder_p.h
        platform/qplatformcamera.cpp platform/qplatformcamera_p.h
        platform/qplatformcameraexposure.cpp platform/qplatformcameraexposure_p.h
        platform/qplatformcamerafocus.cpp platform/qplatformcamerafocus_p.h
        platform/qplatformcameraimagecapture.cpp platform/qplatformcameraimagecapture_p.h
        platform/qplatformcameraimageprocessing.cpp platform/qplatformcameraimageprocessing_p.h
        platform/qplatformmediacapture.cpp platform/qplatformmediacapture_p.h
        platform/qplatformmediadevicemanager.cpp platform/qplatformmediadevicemanager_p.h
        platform/qplatformmediaencoder.cpp platform/qplatformmediaencoder_p.h
        platform/qplatformmediaformatinfo.cpp  platform/qplatformmediaformatinfo_p.h
        platform/qplatformmediaintegration.cpp platform/qplatformmediaintegration_p.h
        platform/qplatformmediaplayer.cpp platform/qplatformmediaplayer_p.h
        playback/qmediaplayer.cpp playback/qmediaplayer.h playback/qmediaplayer_p.h
        playback/qmediaplaylist.cpp playback/qmediaplaylist.h playback/qmediaplaylist_p.h
        playback/qplaylistfileparser.cpp playback/qplaylistfileparser_p.h
        qmediadevicemanager.cpp qmediadevicemanager.h
        qmediaenumdebug.h
        qmediaformat.cpp  qmediaformat.h
        qmediametadata.cpp qmediametadata.h
        qmediapluginloader.cpp qmediapluginloader_p.h
        qmediastoragelocation.cpp qmediastoragelocation_p.h
        qmediatimerange.cpp qmediatimerange.h
        qmultimediautils.cpp qmultimediautils_p.h
        qtmultimediaglobal.h qtmultimediaglobal_p.h
        recording/qmediacapturesession.cpp recording/qmediacapturesession.h
        recording/qmediaencoder.cpp recording/qmediaencoder.h recording/qmediaencoder_p.h
        recording/qmediaencodersettings.cpp recording/qmediaencodersettings.h
        recording/qmediarecorder.cpp recording/qmediarecorder.h recording/qmediarecorder_p.h
        video/qabstractvideobuffer.cpp video/qabstractvideobuffer_p.h
        video/qabstractvideofilter.cpp video/qabstractvideofilter.h
        video/qabstractvideosurface.cpp video/qabstractvideosurface.h
        video/qimagevideobuffer.cpp video/qimagevideobuffer_p.h
        video/qmemoryvideobuffer.cpp video/qmemoryvideobuffer_p.h
        video/qpaintervideosurface.cpp video/qpaintervideosurface_p.h
        video/qvideoframe.cpp video/qvideoframe.h
        video/qvideosink.cpp video/qvideosink.h
        video/qvideoframeconversionhelper.cpp video/qvideoframeconversionhelper_p.h
        video/qvideooutputorientationhandler.cpp video/qvideooutputorientationhandler_p.h
        video/qvideosurfaceformat.cpp video/qvideosurfaceformat.h
        video/qvideosurfaces.cpp video/qvideosurfaces_p.h
    INCLUDE_DIRECTORIES
        audio
        camera
        controls
        platform
        playback
        recording
        video
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::Network
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::GuiPrivate
)

# ### Get rid of me!
qt_extend_target(Multimedia CONDITION TARGET Qt::OpenGL
    LIBRARIES
        Qt::OpenGL
)

qt_internal_add_simd_part(Multimedia SIMD sse2
    SOURCES
        video/qvideoframeconversionhelper_sse2.cpp
)

qt_internal_add_simd_part(Multimedia SIMD ssse3
    SOURCES
        video/qvideoframeconversionhelper_ssse3.cpp
)

qt_internal_add_simd_part(Multimedia SIMD avx2
    SOURCES
        video/qvideoframeconversionhelper_avx2.cpp
)


if(ANDROID)
    set_property(TARGET Multimedia APPEND PROPERTY QT_ANDROID_BUNDLED_JAR_DEPENDENCIES
        jar/QtAndroidMultimedia.jar:org.qtproject.qt.android.multimedia.QtMultimediaUtils
    )
    set_property(TARGET Multimedia APPEND PROPERTY QT_ANDROID_LIB_DEPENDENCIES
        lib/libQt5MultimediaQuick.so:Qt5Quick
        plugins/mediaservice/libplugins_mediaservice_qtmedia_android.so
    )
    set_property(TARGET Multimedia APPEND PROPERTY QT_ANDROID_BUNDLED_FILES
        lib/libQt5MultimediaQuick.so
    )
    set_property(TARGET Multimedia APPEND PROPERTY QT_ANDROID_PERMISSIONS
        android.permission.CAMERA android.permission.RECORD_AUDIO
    )
endif()

#### Keys ignored in scope 1:.:.:multimedia.pro:<TRUE>:
# ANDROID_FEATURES = "android.hardware.camera" "android.hardware.camera.autofocus" "android.hardware.microphone"
# MODULE_WINRT_CAPABILITIES_DEVICE = "microphone" "webcam"

## Scopes:
#####################################################################

qt_internal_extend_target(Multimedia CONDITION WIN32
    SOURCES
        platform/windows/audio/qwindowsaudiodeviceinfo.cpp platform/windows/audio/qwindowsaudiodeviceinfo_p.h
        platform/windows/audio/qwindowsaudioinput.cpp platform/windows/audio/qwindowsaudioinput_p.h
        platform/windows/audio/qwindowsaudiooutput.cpp platform/windows/audio/qwindowsaudiooutput_p.h
        platform/windows/audio/qwindowsaudioutils.cpp platform/windows/audio/qwindowsaudioutils_p.h
        platform/windows/decoder/mfaudiodecodercontrol.cpp platform/windows/decoder/mfaudiodecodercontrol_p.h
        platform/windows/decoder/mfdecodersourcereader.cpp platform/windows/decoder/mfdecodersourcereader_p.h
        platform/windows/evr/evrcustompresenter.cpp platform/windows/evr/evrcustompresenter_p.h
        platform/windows/evr/evrd3dpresentengine.cpp platform/windows/evr/evrd3dpresentengine_p.h
        platform/windows/evr/evrdefs.cpp platform/windows/evr/evrdefs_p.h
        platform/windows/evr/evrhelpers.cpp platform/windows/evr/evrhelpers_p.h
        platform/windows/evr/evrvideowindowcontrol.cpp platform/windows/evr/evrvideowindowcontrol_p.h
        platform/windows/mfstream.cpp platform/windows/mfstream_p.h
        platform/windows/player/mfactivate.cpp platform/windows/player/mfactivate_p.h
        platform/windows/player/mfevrvideowindowcontrol.cpp platform/windows/player/mfevrvideowindowcontrol_p.h
        platform/windows/player/mfmetadata.cpp platform/windows/player/mfmetadata_p.h
        platform/windows/player/mfplayercontrol.cpp platform/windows/player/mfplayercontrol_p.h
        platform/windows/player/mfplayersession.cpp platform/windows/player/mfplayersession_p.h
        platform/windows/player/mftvideo.cpp platform/windows/player/mftvideo_p.h
        platform/windows/player/mfvideorenderercontrol.cpp platform/windows/player/mfvideorenderercontrol_p.h
        platform/windows/player/samplegrabber.cpp platform/windows/player/samplegrabber_p.h
        platform/windows/qwindowsdevicemanager.cpp platform/windows/qwindowsdevicemanager_p.h
        platform/windows/qwindowsformatinfo.cpp platform/windows/qwindowsformatinfo_p.h
        platform/windows/qwindowsintegration.cpp platform/windows/qwindowsintegration_p.h
        platform/windows/sourceresolver.cpp platform/windows/sourceresolver_p.h
    INCLUDE_DIRECTORIES
        platform/windows/decoder
        platform/windows/evr
        platform/windows/player
    LIBRARIES
        Qt::GuiPrivate
        uuid
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Network
        WMF::WMF
        d3d9
        dxva2
        evr
        gdi32
        mf
        mfplat
        mfreadwrite
        mfuuid
        ole32
        oleaut32
        strmiids
        user32
        winmm
        wmcodecdspuuid
    PRIVATE_MODULE_INTERFACE
        Qt::GuiPrivate
)

qt_internal_extend_target(Multimedia CONDITION QT_FEATURE_gstreamer
    SOURCES
        platform/gstreamer/audio/qaudiodeviceinfo_gstreamer.cpp platform/gstreamer/audio/qaudiodeviceinfo_gstreamer_p.h
        platform/gstreamer/audio/qaudioinput_gstreamer.cpp platform/gstreamer/audio/qaudioinput_gstreamer_p.h
        platform/gstreamer/audio/qaudiooutput_gstreamer.cpp platform/gstreamer/audio/qaudiooutput_gstreamer_p.h
        platform/gstreamer/audio/qgstreameraudiodecoder.cpp platform/gstreamer/audio/qgstreameraudiodecoder_p.h
        platform/gstreamer/common/qgst_p.h
        platform/gstreamer/common/qgstappsrc.cpp platform/gstreamer/common/qgstappsrc_p.h
        platform/gstreamer/common/qgstreameraudioinput.cpp platform/gstreamer/common/qgstreameraudioinput_p.h
        platform/gstreamer/common/qgstreameraudiooutput.cpp platform/gstreamer/common/qgstreameraudiooutput_p.h
        platform/gstreamer/common/qgstreamerbufferprobe.cpp platform/gstreamer/common/qgstreamerbufferprobe_p.h
        platform/gstreamer/common/qgstreamerbushelper.cpp platform/gstreamer/common/qgstreamerbushelper_p.h
        platform/gstreamer/common/qgstreamermetadata.cpp platform/gstreamer/common/qgstreamermetadata_p.h
        platform/gstreamer/common/qgstreamermessage.cpp platform/gstreamer/common/qgstreamermessage_p.h
        platform/gstreamer/common/qgstreamermediaplayer.cpp platform/gstreamer/common/qgstreamermediaplayer_p.h
        platform/gstreamer/common/qgstreamervideooutput.cpp platform/gstreamer/common/qgstreamervideooutput_p.h
        platform/gstreamer/common/qgstreamervideooverlay.cpp platform/gstreamer/common/qgstreamervideooverlay_p.h
        platform/gstreamer/common/qgstreamervideorenderer.cpp platform/gstreamer/common/qgstreamervideorenderer_p.h
        platform/gstreamer/common/qgstreamervideowindow.cpp platform/gstreamer/common/qgstreamervideowindow_p.h
        platform/gstreamer/common/qgstutils.cpp platform/gstreamer/common/qgstutils_p.h
        platform/gstreamer/common/qgstvideobuffer.cpp platform/gstreamer/common/qgstvideobuffer_p.h
        platform/gstreamer/common/qgstvideorendererplugin.cpp platform/gstreamer/common/qgstvideorendererplugin_p.h
        platform/gstreamer/common/qgstvideorenderersink.cpp platform/gstreamer/common/qgstvideorenderersink_p.h
        platform/gstreamer/qgstreamerdevicemanager.cpp platform/gstreamer/qgstreamerdevicemanager_p.h
        platform/gstreamer/qgstreamerformatinfo.cpp platform/gstreamer/qgstreamerformatinfo_p.h
        platform/gstreamer/qgstreamerintegration.cpp platform/gstreamer/qgstreamerintegration_p.h
        platform/gstreamer/mediacapture/qgstreamercamera.cpp platform/gstreamer/mediacapture/qgstreamercamera_p.h
        platform/gstreamer/mediacapture/qgstreamercameraimageprocessing.cpp platform/gstreamer/mediacapture/qgstreamercameraimageprocessing_p.h
        platform/gstreamer/mediacapture/qgstreamercameraexposure.cpp platform/gstreamer/mediacapture/qgstreamercameraexposure_p.h
        platform/gstreamer/mediacapture/qgstreamercamerafocus.cpp platform/gstreamer/mediacapture/qgstreamercamerafocus_p.h
        platform/gstreamer/mediacapture/qgstreamermediacapture.cpp platform/gstreamer/mediacapture/qgstreamermediacapture_p.h
        platform/gstreamer/mediacapture/qgstreamercameraimagecapture.cpp platform/gstreamer/mediacapture/qgstreamercameraimagecapture_p.h
        platform/gstreamer/mediacapture/qgstreamermediaencoder.cpp platform/gstreamer/mediacapture/qgstreamermediaencoder_p.h
    DEFINES
        GLIB_VERSION_MIN_REQUIRED=GLIB_VERSION_2_26
    INCLUDE_DIRECTORIES
        platform/gstreamer/mediaplayer
        platform/gstreamer/mediacapture
    LIBRARIES
        GStreamer::GStreamer # special case
        GStreamer::App # special case
)

qt_internal_extend_target(Multimedia CONDITION QT_FEATURE_gstreamer AND QT_FEATURE_gstreamer_photography
    LIBRARIES
        -lgstphotography-1.0#GStreamer::Photography # special case
)

qt_internal_extend_target(Multimedia CONDITION QT_FEATURE_gstreamer AND QT_FEATURE_gstreamer_gl
    PUBLIC_LIBRARIES
        GStreamer::Gl # special case
)

qt_internal_extend_target(Multimedia CONDITION ANDROID AND QT_FEATURE_gstreamer
    LIBRARIES
        # Remove: L$ENV{GSTREAMER_ROOT_ANDROID}/armv7/lib
        # Remove: Wl,--_p.hole-archive
        # Remove: Wl,--no-_p.hole-archive
        WrapIconv::WrapIconv
        ffi
        glib-2.0
        gmodule-2.0
        gobject-2.0
        gstapp-1.0
        gstaudio-1.0
        gstbase-1.0
        gstpbutils-1.0
        gstreamer-1.0
        gsttag-1.0
        gstvideo-1.0
        intl
        orc-0.4
)

qt_internal_extend_target(Multimedia CONDITION QT_FEATURE_pulseaudio
    SOURCES
        platform/pulseaudio/qaudiodeviceinfo_pulse.cpp platform/pulseaudio/qaudiodeviceinfo_pulse_p.h
        platform/pulseaudio/qaudioengine_pulse.cpp platform/pulseaudio/qaudioengine_pulse_p.h
        platform/pulseaudio/qaudioinput_pulse.cpp platform/pulseaudio/qaudioinput_pulse_p.h
        platform/pulseaudio/qaudiooutput_pulse.cpp platform/pulseaudio/qaudiooutput_pulse_p.h
        platform/pulseaudio/qpulseaudiodevicemanager.cpp platform/pulseaudio/qpulseaudiodevicemanager_p.h
        platform/pulseaudio/qpulseaudiointegration.cpp platform/pulseaudio/qpulseaudiointegration_p.h
        platform/pulseaudio/qpulsehelpers.cpp platform/pulseaudio/qpulsehelpers_p.h
    LIBRARIES
        WrapPulseAudio::WrapPulseAudio
)

qt_internal_extend_target(Multimedia CONDITION QT_FEATURE_alsa
    SOURCES
        platform/alsa/qalsaaudiodeviceinfo.cpp platform/alsa/qalsaaudiodeviceinfo_p.h
        platform/alsa/qalsaaudioinput.cpp platform/alsa/qalsaaudioinput_p.h
        platform/alsa/qalsaaudiooutput.cpp platform/alsa/qalsaaudiooutput_p.h
        platform/alsa/qalsadevicemanager.cpp platform/alsa/qalsadevicemanager_p.h
        platform/alsa/qalsaintegration.cpp platform/alsa/qalsaintegration_p.h
    LIBRARIES
        ALSA::ALSA
)

qt_internal_extend_target(Multimedia CONDITION ANDROID
    SOURCES
        platform/android/audio/qopenslesaudioinput.cpp platform/android/audio/qopenslesaudioinput_p.h
        platform/android/audio/qopenslesaudiooutput.cpp platform/android/audio/qopenslesaudiooutput_p.h
        platform/android/audio/qopenslesdeviceinfo.cpp platform/android/audio/qopenslesdeviceinfo_p.h
        platform/android/audio/qopenslesengine.cpp platform/android/audio/qopenslesengine_p.h
        platform/android/common/qandroidglobal_p.h
        platform/android/common/qandroidmultimediautils.cpp platform/android/common/qandroidmultimediautils_p.h
        platform/android/common/qandroidvideooutput.cpp platform/android/common/qandroidvideooutput_p.h
        platform/android/mediacapture/qandroidcameracontrol.cpp platform/android/mediacapture/qandroidcameracontrol_p.h
        platform/android/mediacapture/qandroidcameraexposurecontrol.cpp platform/android/mediacapture/qandroidcameraexposurecontrol_p.h
        platform/android/mediacapture/qandroidcamerafocuscontrol.cpp platform/android/mediacapture/qandroidcamerafocuscontrol_p.h
        platform/android/mediacapture/qandroidcameraimagecapturecontrol.cpp platform/android/mediacapture/qandroidcameraimagecapturecontrol_p.h
        platform/android/mediacapture/qandroidcameraimageprocessingcontrol.cpp platform/android/mediacapture/qandroidcameraimageprocessingcontrol_p.h
        platform/android/mediacapture/qandroidcamerasession.cpp platform/android/mediacapture/qandroidcamerasession_p.h
        platform/android/mediacapture/qandroidcameravideorenderercontrol.cpp platform/android/mediacapture/qandroidcameravideorenderercontrol_p.h
        platform/android/mediacapture/qandroidcaptureservice.cpp platform/android/mediacapture/qandroidcaptureservice_p.h
        platform/android/mediacapture/qandroidcapturesession.cpp platform/android/mediacapture/qandroidcapturesession_p.h
        platform/android/mediacapture/qandroidmediaencoder.cpp platform/android/mediacapture/qandroidmediaencoder_p.h
        platform/android/mediaplayer/qandroidmediaplayercontrol.cpp platform/android/mediaplayer/qandroidmediaplayercontrol_p.h
        platform/android/mediaplayer/qandroidmediaplayervideorenderercontrol.cpp platform/android/mediaplayer/qandroidmediaplayervideorenderercontrol_p.h
        platform/android/mediaplayer/qandroidmetadata.cpp platform/android/mediaplayer/qandroidmetadata_p.h
        platform/android/qandroiddevicemanager.cpp platform/android/qandroiddevicemanager_p.h
        platform/android/qandroidformatsinfo.cpp  platform/android/qandroidformatsinfo_p.h
        platform/android/qandroidintegration.cpp platform/android/qandroidintegration_p.h
        platform/android/wrappers/jni/androidcamera.cpp platform/android/wrappers/jni/androidcamera_p.h
        platform/android/wrappers/jni/androidmediametadataretriever.cpp platform/android/wrappers/jni/androidmediametadataretriever_p.h
        platform/android/wrappers/jni/androidmediaplayer.cpp platform/android/wrappers/jni/androidmediaplayer_p.h
        platform/android/wrappers/jni/androidmediarecorder.cpp platform/android/wrappers/jni/androidmediarecorder_p.h
        platform/android/wrappers/jni/androidmultimediautils.cpp platform/android/wrappers/jni/androidmultimediautils_p.h
        platform/android/wrappers/jni/androidsurfacetexture.cpp platform/android/wrappers/jni/androidsurfacetexture_p.h
        platform/android/wrappers/jni/androidsurfaceview.cpp platform/android/wrappers/jni/androidsurfaceview_p.h
    INCLUDE_DIRECTORIES
        platform/android/common
        platform/android/mediacapture
        platform/android/mediaplayer
        platform/android/wrappers/jni
    LIBRARIES
        Qt::CorePrivate
    PUBLIC_LIBRARIES
        OpenSLES
        Qt::Core
        Qt::Network
        Qt::OpenGL
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
)

qt_internal_extend_target(Multimedia CONDITION WIN32 AND NOT TARGET Qt::OpenGL
    LIBRARIES
        gdi32
        user32
)

qt_internal_extend_target(Multimedia CONDITION TARGET Qt::Widgets AND WIN32
    PUBLIC_LIBRARIES
        Qt::Widgets
)

qt_internal_extend_target(Multimedia CONDITION APPLE AND NOT WATCHOS
    SOURCES
        platform/darwin/audio/qcoreaudiodeviceinfo.mm platform/darwin/audio/qcoreaudiodeviceinfo_p.h
        platform/darwin/audio/qcoreaudioinput.mm platform/darwin/audio/qcoreaudioinput_p.h
        platform/darwin/audio/qcoreaudiooutput.mm platform/darwin/audio/qcoreaudiooutput_p.h
        platform/darwin/audio/qcoreaudioutils.mm platform/darwin/audio/qcoreaudioutils_p.h
        platform/darwin/mediaplayer/avfmediaplayer.mm platform/darwin/mediaplayer/avfmediaplayer_p.h
        platform/darwin/mediaplayer/avfmetadata.mm platform/darwin/mediaplayer/avfmetadata_p.h
        platform/darwin/mediaplayer/avfvideooutput.mm platform/darwin/mediaplayer/avfvideooutput_p.h
        platform/darwin/mediaplayer/avfvideowindowcontrol.mm platform/darwin/mediaplayer/avfvideowindowcontrol_p.h
        platform/darwin/qdarwindevicemanager.mm platform/darwin/qdarwindevicemanager_p.h
        platform/darwin/qdarwinformatsinfo.mm platform/darwin/qdarwinformatsinfo_p.h
        platform/darwin/qdarwinintegration.cpp platform/darwin/qdarwinintegration_p.h
    PUBLIC_LIBRARIES
        ${FWAudioToolbox}
        ${FWCoreAudio}
        ${FWCoreFoundation}
        ${FWCoreMedia}
        ${FWCoreVideo}
        ${FWFoundation}
        ${FWMetal}
        ${FWQuartzCore}
        AVFoundation::AVFoundation
        Qt::Network
        Qt::OpenGL
)

qt_internal_extend_target(Multimedia CONDITION MACOS AND NOT WATCHOS
    PUBLIC_LIBRARIES
        ${FWAppKit}
        ${FWAudioUnit}
)

qt_internal_extend_target(Multimedia CONDITION IOS AND NOT WATCHOS
    PUBLIC_LIBRARIES
        ${FWCoreGraphics}
        ${FWCoreVideo}
)

qt_internal_extend_target(Multimedia CONDITION APPLE AND NOT TVOS AND NOT WATCHOS
    SOURCES
        platform/darwin/camera/avfcameracontrol.mm platform/darwin/camera/avfcameracontrol_p.h
        platform/darwin/camera/avfcameradebug_p.h
        platform/darwin/camera/avfcameraexposurecontrol.mm platform/darwin/camera/avfcameraexposurecontrol_p.h
        platform/darwin/camera/avfcamerafocuscontrol.mm platform/darwin/camera/avfcamerafocuscontrol_p.h
        platform/darwin/camera/avfcameraimageprocessingcontrol.mm platform/darwin/camera/avfcameraimageprocessingcontrol_p.h
        platform/darwin/camera/avfcamerarenderercontrol.mm platform/darwin/camera/avfcamerarenderercontrol_p.h
        platform/darwin/camera/avfcameraservice.mm platform/darwin/camera/avfcameraservice_p.h
        platform/darwin/camera/avfcamerasession.mm platform/darwin/camera/avfcamerasession_p.h
        platform/darwin/camera/avfcamerautility.mm platform/darwin/camera/avfcamerautility_p.h
        platform/darwin/camera/avfcamerawindowcontrol.mm platform/darwin/camera/avfcamerawindowcontrol_p.h
        platform/darwin/camera/avfimagecapturecontrol.mm platform/darwin/camera/avfimagecapturecontrol_p.h
        platform/darwin/camera/avfstoragelocation.mm platform/darwin/camera/avfstoragelocation_p.h
)

qt_internal_extend_target(Multimedia CONDITION APPLE AND NOT TVOS AND NOT WATCHOS
    SOURCES
        platform/darwin/camera/avfmediaassetwriter.mm platform/darwin/camera/avfmediaassetwriter_p.h
        platform/darwin/camera/avfmediarecordercontrol.mm platform/darwin/camera/avfmediarecordercontrol_p.h
)

qt_internal_extend_target(Multimedia CONDITION APPLE AND NOT WATCHOS AND (IOS OR TVOS)
    SOURCES
        platform/darwin/audio/qcoreaudiosessionmanager.mm platform/darwin/audio/qcoreaudiosessionmanager_p.h
    PUBLIC_LIBRARIES
        ${FWAVFoundation}
        ${FWFoundation}
)

qt_internal_extend_target(Multimedia CONDITION APPLE AND NOT IOS AND NOT TVOS AND NOT WATCHOS
    PUBLIC_LIBRARIES
        ${FWAppKit}
        ${FWApplicationServices}
        ${FWAudioUnit}
)

qt_internal_extend_target(Multimedia CONDITION APPLE AND QT_FEATURE_opengl AND NOT WATCHOS AND (IOS OR TVOS)
    SOURCES
        platform/darwin/mediaplayer/avfdisplaylink.mm platform/darwin/mediaplayer/avfdisplaylink_p.h
        platform/darwin/mediaplayer/avfvideoframerenderer_ios.mm platform/darwin/mediaplayer/avfvideoframerenderer_ios_p.h
        platform/darwin/mediaplayer/avfvideorenderercontrol.mm platform/darwin/mediaplayer/avfvideorenderercontrol_p.h
)

qt_internal_extend_target(Multimedia CONDITION APPLE AND QT_FEATURE_opengl AND NOT IOS AND NOT TVOS AND NOT WATCHOS
    SOURCES
        platform/darwin/mediaplayer/avfdisplaylink.mm platform/darwin/mediaplayer/avfdisplaylink_p.h
        platform/darwin/mediaplayer/avfvideoframerenderer.mm platform/darwin/mediaplayer/avfvideoframerenderer_p.h
        platform/darwin/mediaplayer/avfvideorenderercontrol.mm platform/darwin/mediaplayer/avfvideorenderercontrol_p.h
)

qt_internal_extend_target(Multimedia CONDITION QNX
    SOURCES
        platform/qnx/audio/neutrinoserviceplugin.cpp platform/qnx/audio/neutrinoserviceplugin_p.h
        platform/qnx/audio/qnxaudiodeviceinfo.cpp platform/qnx/audio/qnxaudiodeviceinfo_p.h
        platform/qnx/audio/qnxaudioinput.cpp platform/qnx/audio/qnxaudioinput_p.h
        platform/qnx/audio/qnxaudiooutput.cpp platform/qnx/audio/qnxaudiooutput_p.h
        platform/qnx/audio/qnxaudioutils.cpp platform/qnx/audio/qnxaudioutils_p.h
        platform/qnx/camera/bbcameraaudioencodersettingscontrol.cpp platform/qnx/camera/bbcameraaudioencodersettingscontrol_p.h
        platform/qnx/camera/bbcameracontrol.cpp platform/qnx/camera/bbcameracontrol_p.h
        platform/qnx/camera/bbcameraexposurecontrol.cpp platform/qnx/camera/bbcameraexposurecontrol_p.h
        platform/qnx/camera/bbcamerafocuscontrol.cpp platform/qnx/camera/bbcamerafocuscontrol_p.h
        platform/qnx/camera/bbcameraimagecapturecontrol.cpp platform/qnx/camera/bbcameraimagecapturecontrol_p.h
        platform/qnx/camera/bbcameraimageprocessingcontrol.cpp platform/qnx/camera/bbcameraimageprocessingcontrol_p.h
        platform/qnx/camera/bbcameramediarecordercontrol.cpp platform/qnx/camera/bbcameramediarecordercontrol_p.h
        platform/qnx/camera/bbcameraorientatio_p.handler.cpp platform/qnx/camera/bbcameraorientatio_p.handler.h
        platform/qnx/camera/bbcameraservice.cpp platform/qnx/camera/bbcameraservice_p.h
        platform/qnx/camera/bbcamerasession.cpp platform/qnx/camera/bbcamerasession_p.h
        platform/qnx/camera/bbcameravideoencodersettingscontrol.cpp platform/qnx/camera/bbcameravideoencodersettingscontrol_p.h
        platform/qnx/camera/bbmediastoragelocation.cpp platform/qnx/camera/bbmediastoragelocation_p.h
        platform/qnx/camera/bbvideorenderercontrol.cpp platform/qnx/camera/bbvideorenderercontrol_p.h
        platform/qnx/common/windowgrabber.cpp platform/qnx/common/windowgrabber_p.h
        platform/qnx/mediaplayer/mmrenderermediaplayercontrol.cpp platform/qnx/mediaplayer/mmrenderermediaplayercontrol_p.h
        platform/qnx/mediaplayer/mmrenderermetadata.cpp platform/qnx/mediaplayer/mmrenderermetadata_p.h
        platform/qnx/mediaplayer/mmrendererplayervideorenderercontrol.cpp platform/qnx/mediaplayer/mmrendererplayervideorenderercontrol_p.h
        platform/qnx/mediaplayer/mmrendererutil.cpp platform/qnx/mediaplayer/mmrendererutil_p.h
        platform/qnx/mediaplayer/mmrenderervideowindowcontrol.cpp platform/qnx/mediaplayer/mmrenderervideowindowcontrol_p.h
        platform/qnx/mediaplayer/mmrevent_p.hread.cpp platform/qnx/mediaplayer/mmrevent_p.hread.h
        platform/qnx/mediaplayer/mmreventmediaplayercontrol.cpp platform/qnx/mediaplayer/mmreventmediaplayercontrol_p.h
        platform/qnx/qqnxdevicemanager.cpp platform/qnx/qqnxdevicemanager_p.h
        platform/qnx/qqnxintegration.cpp platform/qnx/qqnxintegration_p.h
    INCLUDE_DIRECTORIES
        platform/qnx/camera
        platform/qnx/common
        platform/qnx/mediaplayer
    PUBLIC_LIBRARIES
        MMRenderer::MMRenderer
        asound
        audio_manager
        camapi
)
qt_internal_add_docs(Multimedia
    doc/qtmultimedia.qdocconf
)

