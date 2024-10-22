QT       += core gui core5compat network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DESTDIR = ../bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

INCLUDEPATH += ../inc

SOURCES += \
    ../src/FuncUI.cpp \
    ../src/main.cpp \
    ../src/mainwindow.cpp

HEADERS += \
    ../inc/FuncUI.h \
    ../inc/mainwindow.h

FORMS += \
    ../ui/mainwindow.ui
RC_FILE = ../res/logo.rc
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
