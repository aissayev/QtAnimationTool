osx:  DESIGNER_PATH = QmlDesigner
else: DESIGNER_PATH = qmldesigner

DESTDIR = $$IDE_PLUGIN_PATH/$$DESIGNER_PATH

target.path = $$INSTALL_PLUGIN_PATH/$$DESIGNER_PATH
INSTALLS += target
