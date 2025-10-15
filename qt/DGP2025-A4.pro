NAME = DGP2025-A4

#
# vim:filetype=qmake sw=4 ts=4 expandtab nospell
#

# CONFIG += static

unix:!macx:CONFIG += USE_UNIX_DAEMONIZE

# CONFIG += c++11 c++14 c++17
CONFIG += sdk_no_version_check

##########################################################################

BASEDIR = ..
TOPDIR = $$BASEDIR/..

UI_DIR    = GeneratedFiles
DESTDIR   = $$BASEDIR/bin
SOURCEDIR = $$BASEDIR/src
FORMSDIR  = $$BASEDIR/forms
ASSETSDIR = $$BASEDIR/assets

CONFIG += qt
QT += core
QT += widgets
QT += openglwidgets

DEFINES += HAVE_IMG
DEFINES += HAVE_HOMEWORK

win32 {
    DEFINES += NOMINMAX _CRT_SECURE_NO_WARNINGS
    DEFINES += _SCL_SECURE_NO_WARNINGS _USE_MATH_DEFINES
    QMAKE_CXXFLAGS_WARN_ON += -W3 -wd4396 -wd4100 -wd4996
    QMAKE_LFLAGS += /INCREMENTAL:NO
    DSHOW_LIBS = -lStrmiids -lVfw32 -lOle32 -lOleAut32 -lopengl32
}

unix:!macx {
    QMAKE_LFLAGS += -Wl
    #QMAKE_CXXFLAGS += -g
}

macx {
    # OTHER_CPLUSPLUSFLAGS += -std=gnu++11
    # QMAKE_CXXFLAGS += -std=gnu++11
    OTHER_CPLUSPLUSFLAGS += -feliminate-unused-debug-types
    # LIBS += -framework Foundation -framework QTKit 
    LIBS += -framework CoreFoundation -framework IOkit
}

CONFIG(release, debug|release) {
    TARGET = $$NAME
} else {
    TARGET = $${NAME}_d
}
CONFIG += console

# message(LIBS = $$LIBS)

include($${NAME}.pri)
