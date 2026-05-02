# 答案之书项目文件
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 不设置特殊编译选项，Qt 6 默认就是 UTF-8

# 项目源文件
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    answerdata.cpp \
    databasehandler.cpp

# 项目头文件
HEADERS += \
    mainwindow.h \
    answerdata.h \
    databasehandler.h

# UI文件
FORMS += \
    mainwindow.ui

# 默认规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 资源文件
RESOURCES +=

# 数据文件（不编译，仅用于运行时）
OTHER_FILES += \
    data/answers.txt
