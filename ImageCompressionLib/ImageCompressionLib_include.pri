win32:CONFIG(release, debug|release) {
    exists($$OUT_PWD/../../ImageCompressionLib/release/ -lImageCompressionLib) {
	LIBS += -L$$OUT_PWD/../../ImageCompressionLib/release/ -lImageCompressionLib
    } else {
	LIBS += -L$$OUT_PWD/../ImageCompressionLib/release/ -lImageCompressionLib
    }
}
else:win32:CONFIG(debug, debug|release) {
    exists($$OUT_PWD/../../ImageCompressionLib/debug/) {
	LIBS += -L$$OUT_PWD/../../ImageCompressionLib/debug/ -lImageCompressionLib
    } else {
	LIBS += -L$$OUT_PWD/../ImageCompressionLib/debug/ -lImageCompressionLib
    }
}

INCLUDEPATH += \
	    $$PWD/include \
	    $$PWD/include/image \
	    $$PWD/include/imageCompression \
	    $$PWD/include/utilities \
	    $$PWD/include/utilities/ioStream

DEPENDPATH += \
	    $$PWD/include \
	    $$PWD/include/image \
	    $$PWD/include/imageCompression \
	    $$PWD/include/utilities \
	    $$PWD/include/utilities/ioStream
