# Generated from qdeclarativevideooutput_window.pro.

#####################################################################
## tst_qdeclarativevideooutput_window Test:
#####################################################################

qt_add_test(tst_qdeclarativevideooutput_window
    SOURCES
        tst_qdeclarativevideooutput_window.cpp
    INCLUDE_DIRECTORIES
        ../../../../src/imports/multimedia
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::MultimediaPrivate
        Qt::MultimediaQuickPrivate
        Qt::Qml
        Qt::Quick
)

# Resources:
set(qml_resource_files
    "main.qml"
)

qt_add_resource(tst_qdeclarativevideooutput_window "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

