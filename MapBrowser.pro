#-------------------------------------------------
#
# Project created by QtCreator 2011-05-05T00:22:27
#
#-------------------------------------------------

QT       += core gui webkit xml

TARGET = MapBrowser
TEMPLATE = app


SOURCES += main.cpp\
        mapbrowser.cpp \
    mapfolder.cpp \
    maptree.cpp \
    mapitem.cpp \
    mapaction.cpp \
    mapfolderdialog.cpp \
    mapitemdialog.cpp \
    maptohtml.cpp \
    maploader.cpp \
    mapfoldervalidator.cpp \
    mappreview.cpp \
    mapwebview.cpp

HEADERS  += mapbrowser.h \
    mapfolder.h \
    maptree.h \
    mapitem.h \
    mapaction.h \
    mapfolderdialog.h \
    mapitemdialog.h \
    maptohtml.h \
    maploader.h \
    mapfoldervalidator.h \
    mappreview.h \
    mapwebview.h

FORMS    += mapbrowser.ui \
    mapfolderdialog.ui \
    mapitemdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    mapbrowser_en.ts \
    win32rc.rc

win32:RC_FILE += win32rc.rc
