# answer_book
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Force UTF-8 encoding for all compilers
win32-msvc* {
    QMAKE_CXXFLAGS += /utf-8
    DEFINES += UNICODE _UNICODE
}

win32-g++ {
    QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
    QMAKE_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
    QMAKE_LFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
    DEFINES += UNICODE _UNICODE
}

# Sources
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    answerdata.cpp \
    databasehandler.cpp

# Headers
HEADERS += \
    mainwindow.h \
    answerdata.h \
    databasehandler.h

# UI files
FORMS += \
    mainwindow.ui

# Installation
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Resources
RESOURCES +=

# Other files
OTHER_FILES += \
    data/answers.txt
