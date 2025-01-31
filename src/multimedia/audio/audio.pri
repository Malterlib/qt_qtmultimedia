INCLUDEPATH += audio

PUBLIC_HEADERS += \
           audio/qaudio.h \
           audio/qaudiobuffer.h \
           audio/qaudioformat.h \
           audio/qaudioinput.h \
           audio/qaudiooutput.h \
           audio/qaudiodeviceinfo.h \
           audio/qsoundeffect.h \
           audio/qaudiodecoder.h

PRIVATE_HEADERS += \
           audio/qaudiobuffer_p.h \
           audio/qaudiodeviceinfo_p.h \
           audio/qwavedecoder.h \
           audio/qsamplecache_p.h \
           audio/qaudiohelpers_p.h \
           audio/qaudiosystem_p.h  \

SOURCES += \
           audio/qaudio.cpp \
           audio/qaudioformat.cpp  \
           audio/qaudiodeviceinfo.cpp \
           audio/qaudiooutput.cpp \
           audio/qaudioinput.cpp \
           audio/qaudiosystem.cpp \
           audio/qsoundeffect.cpp \
           audio/qwavedecoder.cpp \
           audio/qsamplecache_p.cpp \
           audio/qaudiobuffer.cpp \
           audio/qaudiodecoder.cpp \
           audio/qaudiohelpers.cpp
