QT *= qml quick core

VPATH += $$PWD

SOURCES += timelineview.cpp \
    timelinewidget.cpp \
    timelinenavigatorentity.cpp \
    timelineqmlbackend.cpp \
    timelinemodel.cpp

HEADERS += timelineview.h \
    timelinewidget.h \
    timelinenavigatorentity.h \
    timelineqmlbackend.h \
    timelinemodel.h

RESOURCES += timeline.qrc
