QT += quick

HEADERS += \
	sources/Application.h

SOURCES += \
	sources/Application.cpp \
	sources/main.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include($$PWD/../ImageCompressionUiComponents/ImageCompressionUiComponents_include.pri)
include($$PWD/../ImageCompressionLib/ImageCompressionLib_include.pri)


WIDGETS_PLUGIN_NAME = PBWidgets
PBWIDGETS_DEPENDENCY_DIRECTORY = "$$OUT_PWD/../PBWidgets/imports/$$WIDGETS_PLUGIN_NAME"

win32:CONFIG(release, debug|release): PBWIDGETS_OUT_DIRECTORY = "$$OUT_PWD/release/$$WIDGETS_PLUGIN_NAME"
else:win32:CONFIG(debug, debug|release): PBWIDGETS_OUT_DIRECTORY = "$$OUT_PWD/debug/$$WIDGETS_PLUGIN_NAME/"

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PBWIDGETS_OUT_DIRECTORY

copy_qmldir.depends = $$PBWIDGETS_DEPENDENCY_DIRECTORY
copy_qmldir.commands = $(COPY_DIR) "$$shell_path($$copy_qmldir.depends)" "$$shell_path($$PBWIDGETS_OUT_DIRECTORY)"

first.depends = $(first) copy_qmldir
export(first.depends)
export(copy_qmldir)
QMAKE_EXTRA_TARGETS += first copy_qmldir

