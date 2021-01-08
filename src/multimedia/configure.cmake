

#### Inputs



#### Libraries



#### Tests


qt_config_compile_test("evr"
                   LABEL "evr.h"
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../config.tests/evr"
)

qt_config_compile_test("gstreamer_encodingprofiles"
                   LABEL "GStreamer encoding-profile.h"
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../config.tests/gstreamer_encodingprofiles"
)

qt_config_compile_test("gpu_vivante"
                   LABEL "Vivante GPU"
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../config.tests/gpu_vivante"
)

qt_config_compile_test("linux_v4l"
                   LABEL "Video for Linux"
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../config.tests/linux_v4l"
)

qt_config_compile_test("wmsdk"
                   LABEL "wmsdk.h"
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../config.tests/wmsdk"
)


#### Features

qt_feature("alsa" PUBLIC PRIVATE
    LABEL "ALSA"
    CONDITION UNIX AND NOT QNX AND libs.alsa AND NOT libs.pulseaudio OR FIXME
)
qt_feature_definition("alsa" "QT_NO_ALSA" NEGATE VALUE "1")
qt_feature("avfoundation" PUBLIC PRIVATE
    LABEL "AVFoundation"
    CONDITION libs.avfoundation OR FIXME
    EMIT_IF APPLE
)
qt_feature_definition("avfoundation" "QT_NO_AVFOUNDATION" NEGATE VALUE "1")
qt_feature("evr" PUBLIC PRIVATE
    LABEL "evr.h"
    CONDITION WIN32 AND TEST_evr
)
qt_feature_definition("evr" "QT_NO_EVR" NEGATE VALUE "1")
qt_feature("gstreamer_1_0" PRIVATE
    LABEL "GStreamer 1.0"
    CONDITION libs.gstreamer_1_0 OR FIXME
    ENABLE INPUT_gstreamer STREQUAL 'yes'
    DISABLE INPUT_gstreamer STREQUAL 'no'
)
qt_feature("gstreamer" PRIVATE
    CONDITION QT_FEATURE_gstreamer_1_0
)
qt_feature("gstreamer_app" PRIVATE
    LABEL "GStreamer App"
    CONDITION ( QT_FEATURE_gstreamer_1_0 AND libs.gstreamer_app_1_0 ) OR FIXME
)
qt_feature("gstreamer_encodingprofiles" PRIVATE
    LABEL "GStreamer encoding-profile.h"
    CONDITION QT_FEATURE_gstreamer AND TEST_gstreamer_encodingprofiles
)
qt_feature("gstreamer_photography" PRIVATE
    LABEL "GStreamer Photography"
    CONDITION ( QT_FEATURE_gstreamer_1_0 AND libs.gstreamer_photography_1_0 ) OR FIXME
)
qt_feature("gstreamer_gl" PRIVATE
    LABEL "GStreamer OpenGL"
    CONDITION QT_FEATURE_opengl AND QT_FEATURE_gstreamer_1_0 AND libs.gstreamer_gl_1_0 OR FIXME
)
qt_feature("gpu_vivante" PRIVATE
    LABEL "Vivante GPU"
    CONDITION QT_FEATURE_gui AND QT_FEATURE_opengles2 AND TEST_gpu_vivante
)
qt_feature("linux_v4l" PRIVATE
    LABEL "Video for Linux"
    CONDITION UNIX AND TEST_linux_v4l
)
qt_feature("mmrenderer" PUBLIC PRIVATE
    LABEL "MMRenderer"
    CONDITION libs.mmrenderer OR FIXME
    EMIT_IF QNX
)
qt_feature_definition("mmrenderer" "QT_NO_MMRENDERER" NEGATE VALUE "1")
qt_feature("pulseaudio" PUBLIC PRIVATE
    LABEL "PulseAudio"
    AUTODETECT UNIX
    CONDITION libs.pulseaudio AND NOT features.gstreamer
)
qt_feature_definition("pulseaudio" "QT_NO_PULSEAUDIO" NEGATE VALUE "1")
qt_feature("wmsdk" PRIVATE
    LABEL "wmsdk.h"
    CONDITION WIN32 AND TEST_wmsdk
)
qt_feature("wmf" PRIVATE
    LABEL "Windows Media Foundation"
    CONDITION WIN32 AND libs.wmf OR FIXME
)
qt_configure_add_summary_section(NAME "Qt Multimedia")
qt_configure_add_summary_entry(ARGS "alsa")
qt_configure_add_summary_entry(ARGS "gstreamer_1_0")
qt_configure_add_summary_entry(ARGS "linux_v4l")
qt_configure_add_summary_entry(ARGS "pulseaudio")
qt_configure_add_summary_entry(ARGS "mmrenderer")
qt_configure_add_summary_entry(ARGS "avfoundation")
qt_configure_add_summary_entry(ARGS "wmf")
qt_configure_end_summary_section() # end of "Qt Multimedia" section