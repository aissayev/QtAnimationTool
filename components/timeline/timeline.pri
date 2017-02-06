QT *= qml quick core

VPATH += $$PWD

SOURCES += timelineview.cpp \
    timelinewidget.cpp \
    $$PWD/timelinenavigatorentity.cpp

HEADERS += timelineview.h \
    timelinewidget.h \
    $$PWD/timelinenavigatorentity.h

RESOURCES += timeline.qrc
