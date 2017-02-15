QT *= qml quick core

VPATH += $$PWD

SOURCES += timelineview.cpp \
    timelinewidget.cpp \
    timelineqmlbackend.cpp \
    timelinemodel.cpp \
    timelineimageprovider.cpp

HEADERS += timelineview.h \
    timelinewidget.h \
    timelineqmlbackend.h \
    timelinemodel.h \
    timelineimageprovider.h

RESOURCES += timeline.qrc
