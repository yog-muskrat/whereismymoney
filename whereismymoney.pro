TEMPLATE = app

QT += widgets sql

OBJECTS_DIR = obj
RCC_DIR = obj
MOC_DIR = obj
UI_DIR = obj

DESTDIR = bin

SOURCES += main.cpp \
	mainwindow.cpp \
	addmonthdialog.cpp \
	sqltools.cpp \
	tools.cpp \
	categorieseditor.cpp \
	wimmmodel.cpp \
	structs.cpp \
	wimmfiltermodel.cpp \
    changevaluecommand.cpp

HEADERS += \
	mainwindow.h \
	addmonthdialog.h \
	structs.h \
	sqltools.h \
	tools.h \
	categorieseditor.h \
	wimmmodel.h \
	wimmfiltermodel.h \
    changevaluecommand.h

DISTFILES += \
	bin/wimm.db

FORMS += \
	mainwindow.ui \
	addmonthdialog.ui \
	categorieseditor.ui

RESOURCES += \
	res.qrc

RC_ICONS = wimm.ico
