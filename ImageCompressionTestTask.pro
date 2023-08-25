TEMPLATE = subdirs

SUBDIRS += \
	ImageCompressionLib \
	ImageCompressionLibTests \
	ImageCompressionUiComponents \
	ImageCompressorUi \
	PBWidgets

ImageCompressionUiComponents.depends = ImageCompressionLib
ImageCompressorUi.depends = ImageCompressionLib ImageCompressionUiComponents PBWidgets
