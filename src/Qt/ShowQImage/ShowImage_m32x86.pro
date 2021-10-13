#-------------------------------------------------
#
# Project created by QtCreator 2017-01-09T10:03:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ShowImage
TEMPLATE = app

win32 {
	LIBS += -L$$PWD/Depends/win32/vs2013shared			-lMVSDKmd
	LIBS += -L$$PWD/Depends/win32/vs2013shared			-lVideoRender
	LIBS += -L$$PWD/Depends/win32/vs2013shared			-lImageConvert
}
else {
	QMAKE_LIBS_OPENGL = 
	DEFINES += QT_NO_DEBUG_OUTPUT LINUX64 QT_NO_OPENGL
	QMAKE_CXXFLAGS_RELEASE += -mssse3
    LIBS += -L./depends/	-lMVSDK
	LIBS += -L../../../lib	-lMVSDK
#	LIBS += -L../../lib/	-llog4cpp
	LIBS += -L./depends/	-lImageConvert
    LIBS += -L../../../lib/GenICam/bin/Linux32_i86 -lGCBase_gcc421_v3_0 -lGenApi_gcc421_v3_0 -lLog_gcc421_v3_0 -llog4cpp_gcc421_v3_0 -lNodeMapData_gcc421_v3_0 -lXmlParser_gcc421_v3_0 -lMathParser_gcc421_v3_0 -lrt -lpthread
}

			
INCLUDEPATH = ./include

SOURCES += src/main.cpp\
           src/CammerWidget.cpp\
           src/form.cpp

HEADERS  += src/CammerWidget.h\
			src/MessageQue.h\
			src/form.h

FORMS    += src/cammerwidget.ui \
			src/form.ui

unix: DESTDIR = ./	






