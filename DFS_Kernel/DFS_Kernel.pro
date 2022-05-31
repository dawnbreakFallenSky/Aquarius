QT += core
QT -= gui

CONFIG += c++11

TARGET = DFS_Kernel
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH = $$PWD/../include

DESTDIR = $$PWD/../Debug

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    APP/clock.cpp \
    APP/main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    APP/clock.h \
    APP/lockfreeQueue.h
