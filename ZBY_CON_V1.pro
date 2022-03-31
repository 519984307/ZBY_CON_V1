QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

CONFIG += skip_target_version_ext
VERSION = 1.0.1.9
QMAKE_TARGET_COPYRIGHT = "Copyright 2020 Shen zhen zhong bai yuan"

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        conv1service.cpp \
        identify.cpp \
        main.cpp \
        tcpclient.cpp \
        tcpserver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    conv1service.h \
    identify.h \
    tcpclient.h \
    tcpserver.h

MOC_DIR=tmp/moc
RCC_DIR=tmp/rcc
UI_DIR=tmp/ui
OBJECTS_DIR=tmp/obj

unix:!macx|win32: LIBS += -L$$PWD/paddleocr/lib/ -lPaddleOCR

INCLUDEPATH += $$PWD/paddleocr/include
DEPENDPATH += $$PWD/paddleocr/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/QtService/lib/ -lQtSolutions_Service-head
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/QtService/lib/ -lQtSolutions_Service-headd
else:unix: LIBS += -L$$PWD/QtService/lib/ -lQtSolutions_Service-head

INCLUDEPATH += $$PWD/QtService
DEPENDPATH += $$PWD/QtService
