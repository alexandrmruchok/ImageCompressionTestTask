TEMPLATE = lib

QT += core

INCLUDEPATH  += \
	sources \
	sources/imageProcessing \
	sources/imagesModels

HEADERS += \
	sources/imageProcessing/FileProcessingTask.h \
	sources/imageProcessing/ImageCompressionManager.h \
	sources/imagesModels/ImagesModel.h \
	sources/imagesModels/ImagesModelItem.h \
	sources/imagesModels/ImagesModelManager.hpp \
	sources/imagesModels/ImagesSortFilterModel.h

SOURCES += \
	sources/imageProcessing/FileProcessingTask.cpp \
	sources/imageProcessing/ImageCompressionManager.cpp \
	sources/imagesModels/ImagesModel.cpp \
	sources/imagesModels/ImagesModelItem.cpp \
	sources/imagesModels/ImagesModelManager.cpp \
	sources/imagesModels/ImagesSortFilterModel.cpp

include($$PWD/../ImageCompressionLib/ImageCompressionLib_include.pri)

DISTFILES += \
	ImageCompressionUiComponents_include.pri
