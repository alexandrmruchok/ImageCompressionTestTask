TEMPLATE = lib

INCLUDEPATH  += \
	include \
	include/image \
	include/imageCompression \
	include/utilities \
	include/utilities/ioStream \

DEFINES += SHAREDLIB_LIBRARY

HEADERS += \
	include/image/BarchImage.h \
	include/image/BmpImage.h \
	include/image/Image.h \
	include/imageCompression/ImageCompression.h \
	include/imageCompression/RawImageData.h \
	include/utilities/ioStream/PointerByBitReader.h \
	include/utilities/ioStream/StreamByBitWriter.h \
	include/utilities/ioStream/dataIO.h \
	include/utilities/libGlobal.h

SOURCES += \
	sources/image/BMPImage.cpp \
	sources/image/BarchImage.cpp \
	sources/imageCompression/ImageCompression.cpp \
	sources/utilities/ioStream/PointerByBitReader.cpp \
	sources/utilities/ioStream/StreamByBitWriter.cpp

DISTFILES += \
	ImageCompressionLib_include.pri
