QT += quickwidgets
QT += widgets-private quick-private quickwidgets-private core-private gui-private #mouse ungrabbing workaround on quickitems
CONFIG += exceptions

INCLUDEPATH += $$PWD
#Path to CLANG or GCC here
INCLUDEPATH += ""
unix:!openbsd:!osx: LIBS += -lrt # posix shared memory

include(../../qtcreatorplugin.pri)

include(designercore/designercore-lib.pri)
include(components/componentcore/componentcore.pri)
include(components/integration/integration.pri)
include(components/propertyeditor/propertyeditor.pri)
include(components/formeditor/formeditor.pri)
include(components/itemlibrary/itemlibrary.pri)
include(components/navigator/navigator.pri)
include(components/stateseditor/stateseditor.pri)
include(components/resources/resources.pri)
include(components/debugview/debugview.pri)
include(components/importmanager/importmanager.pri)
include(qmldesignerextension/qmldesignerextension.pri)
include(qmldesignerplugin.pri)


BUILD_PUPPET_IN_CREATOR_BINPATH = $$(BUILD_PUPPET_IN_CREATOR_BINPATH)
!isEmpty(BUILD_PUPPET_IN_CREATOR_BINPATH) {
    DEFINES += SEARCH_PUPPET_IN_CREATOR_BINPATH
    message("Search puppet in qtcreator bin path!")
}

DISTFILES += \
    components/timeline/timeline.pri \
    components/timeline/arrowdown.png \
    components/timeline/arrowdown@2x.png \
    components/timeline/arrowleft.png \
    components/timeline/arrowleft@2x.png \
    components/timeline/arrowright.png \
    components/timeline/arrowright@2x.png \
    components/timeline/arrowup.png \
    components/timeline/arrowup@2x.png \
    components/timeline/export_checked.png \
    components/timeline/export_checked@2x.png \
    components/timeline/export_unchecked.png \
    components/timeline/export_unchecked@2x.png \
    components/timeline/warning.png

RESOURCES += \
    components/timeline/navigator.qrc

HEADERS += \
    components/formeditor/formeditorsubwindow.h \
    components/timeline/iconcheckboxitemdelegate.h \
    components/timeline/nameitemdelegate.h \
    components/timeline/navigatortreemodel.h \
    components/timeline/navigatortreeview.h \
    components/timeline/navigatorview.h \
    components/timeline/navigatorwidget.h \
    componentsplugin/addtabdesigneraction.h \
    componentsplugin/addtabtotabviewdialog.h \
    componentsplugin/componentsplugin.h \
    componentsplugin/entertabdesigneraction.h \
    componentsplugin/tabviewindexmodel.h \
    designercore/include/bytearraymodifier.h \
    designercore/include/iwidgetplugin.h \
    designercore/include/modificationgroupexception.h \
    designercore/include/modificationgrouptoken.h \
    designercore/include/nodeanchors.h \
    designercore/include/objectpropertybinding.h \
    designercore/include/propertybinding.h \
    qmldesignerextension/qmldesignerextension_global.h \
    qmldesignerextension/qmldesignerextensionconstants.h \
    qtquickplugin/qtquickplugin.h

SOURCES += \
    components/timeline/iconcheckboxitemdelegate.cpp \
    components/timeline/nameitemdelegate.cpp \
    components/timeline/navigatortreemodel.cpp \
    components/timeline/navigatortreeview.cpp \
    components/timeline/navigatorview.cpp \
    components/timeline/navigatorwidget.cpp \
    componentsplugin/addtabdesigneraction.cpp \
    componentsplugin/addtabtotabviewdialog.cpp \
    componentsplugin/componentsplugin.cpp \
    componentsplugin/entertabdesigneraction.cpp \
    componentsplugin/tabviewindexmodel.cpp \
    qtquickplugin/qtquickplugin.cpp
