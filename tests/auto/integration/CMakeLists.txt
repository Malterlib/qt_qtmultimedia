# Generated from integration.pro.

# special case begin
add_subdirectory(qaudiodecoderbackend)
add_subdirectory(qaudiodeviceinfo)
add_subdirectory(qaudioinput)
add_subdirectory(qaudiooutput)
add_subdirectory(qmediaplayerbackend)
add_subdirectory(qsoundeffect)
if(TARGET Qt::Widgets)
    add_subdirectory(qcamerabackend)
endif()
if(TARGET Qt::Quick)
    add_subdirectory(qdeclarativevideooutput)
    add_subdirectory(qdeclarativevideooutput_window)
endif()
# special case end
