TEMPLATE = app

QT += qml quick widgets sql

OBJECTS_DIR = obj
RCC_DIR = obj
MOC_DIR = obj
UI_DIR = obj

# Дефайн для запуска QML-версии приложения
#DEFINES += QMLVERSION

DESTDIR = bin

SOURCES += main.cpp \
	wimmmodel.cpp \
	mainwindow.cpp \
    addmonthdialog.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
	wimmmodel.h \
	mainwindow.h \
    addmonthdialog.h

DISTFILES += \
	bin/wimm.db

FORMS += \
	mainwindow.ui \
    addmonthdialog.ui
