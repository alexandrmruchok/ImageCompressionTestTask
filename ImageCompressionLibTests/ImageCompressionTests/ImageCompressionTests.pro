QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += tst_imagecompressiontests.cpp

include($$PWD/../../ImageCompressionLib/ImageCompressionLib_include.pri)