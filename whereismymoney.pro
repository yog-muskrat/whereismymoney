TEMPLATE = app

QT += qml quick widgets sql

OBJECTS_DIR = obj
RCC_DIR = obj
MOC_DIR = obj
UI_DIR = obj

DESTDIR = bin

SOURCES += main.cpp \
	dbmanager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
	dbmanager.h

DISTFILES += \
    bin/wimm.db
