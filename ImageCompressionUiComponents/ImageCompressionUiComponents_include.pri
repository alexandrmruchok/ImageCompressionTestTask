win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ImageCompressionUiComponents/release/ -lImageCompressionUiComponents
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ImageCompressionUiComponents/debug/ -lImageCompressionUiComponents

INCLUDEPATH += \
		$$PWD/../ImageCompressionUiComponents/sources \
		$$PWD/../ImageCompressionUiComponents/sources/imageProcessing \
		$$PWD/../ImageCompressionUiComponents/sources/imagesModels
DEPENDPATH += \
		$$PWD/../ImageCompressionUiComponents/sources \
		$$PWD/../ImageCompressionUiComponents/sources/imageProcessing \
		$$PWD/../ImageCompressionUiComponents/sources/imagesModels

