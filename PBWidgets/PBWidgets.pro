TEMPLATE = lib
TARGET = pbwidgets
QT += qml quick
CONFIG += qt plugin qmltypes
QMAKE_CXXFLAGS += -Wall

QML_IMPORT_NAME = PBWidgets
QML_IMPORT_MAJOR_VERSION = 1

TARGET = $$qtLibraryTarget($$TARGET)
uri = PBWidgets

HEADERS += \
	sources/PBWidgetsPlugin.h \

RESOURCES += qml/pluginQml.qrc


PLUGIN_DEPENDENCY_DIRECTORY = "$$PWD/qml"
DESTDIR = "imports/$${QML_IMPORT_NAME}"

PluginDependencies.commands = $(COPY_DIR) $$shell_path($$PLUGIN_DEPENDENCY_DIRECTORY) $$shell_path($${DESTDIR}/)
PRE_TARGETDEPS += $$PluginDependencies.target

first.depends = $(first) PluginDependencies
export(first.depends)
export(PluginDependencies)
QMAKE_EXTRA_TARGETS += first PluginDependencies
