QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = write_cd_sample
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        qtutils/checkableproxymodel.cpp \
        qtutils/delayedexecutiontimer.cpp \
        qtutils/modelutils.cpp \
    Logger.cpp \
    WriterCdWidget.cpp \
    ProcessWrapper.cpp \
    CdBurner.cpp \
    CdBurner_p.cpp \
    MainWindow.cpp

HEADERS += \
        qtutils/checkableproxymodel.h \
        qtutils/delayedexecutiontimer.h \
        qtutils/modelutils.h \
    Logger.h \
    WriterCdWidget.h \
    ProcessWrapper.h \
    CdBurner.h \
    CdBurner_p.h \
    Settings.h \
    MainWindow.h

FORMS += \
        mainwindow.ui \
        writecdwidget.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    data/simplebrowser.qrc
