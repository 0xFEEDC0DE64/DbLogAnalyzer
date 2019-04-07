QT += core gui widgets network sql charts

TARGET = loganalyzer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000

CONFIG += c++14
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
QMAKE_CXXFLAGS_RELEASE -= -march=x86-64
QMAKE_CXXFLAGS_RELEASE -= -mtune=generic
QMAKE_CXXFLAGS_RELEASE *= -march=native
QMAKE_CXXFLAGS_RELEASE *= -mtune=native

LIBS += -lz

SOURCES += main.cpp \
    mainwindow.cpp \
    wizard/importwizard.cpp \
    wizard/intropage.cpp \
    wizard/databasepage.cpp \
    wizard/importtypepage.cpp \
    wizard/localimportpage.cpp \
    wizard/conclusionpage.cpp \
    wizard/remoteimportoverviewpage.cpp \
    wizard/tablespage.cpp \
    threads/tablecreatorthread.cpp \
    models/checklistmodel.cpp \
    threads/remotescannerthread.cpp \
    wizard/remoteimportscanpage.cpp \
    threads/importthread.cpp \
    wizard/importprogresspage.cpp \
    dialogs/opendialog.cpp \
    widgets/fileselectionwidget.cpp \
    widgets/databasewidget.cpp \
    gzipdevice.cpp \
    dialogs/graphdialog.cpp \
    models/logmodel.cpp \
    models/progressmodel.cpp

HEADERS += \
    mainwindow.h \
    wizard/importwizard.h \
    wizard/intropage.h \
    wizard/databasepage.h \
    wizard/importtypepage.h \
    wizard/localimportpage.h \
    wizard/conclusionpage.h \
    wizard/remoteimportoverviewpage.h \
    wizard/tablespage.h \
    threads/tablecreatorthread.h \
    models/checklistmodel.h \
    threads/remotescannerthread.h \
    wizard/remoteimportscanpage.h \
    common.h \
    threads/importthread.h \
    wizard/importprogresspage.h \
    dialogs/opendialog.h \
    widgets/fileselectionwidget.h \
    widgets/databasewidget.h \
    gzipdevice.h \
    dialogs/graphdialog.h \
    models/logmodel.h \
    models/progressmodel.h

FORMS += \
    mainwindow.ui \
    dialogs/opendialog.ui \
    widgets/fileselectionwidget.ui \
    widgets/databasewidget.ui \
    wizard/intropage.ui \
    wizard/databasepage.ui \
    dialogs/graphdialog.ui \
    wizard/tablespage.ui

RESOURCES += \
    resources.qrc
