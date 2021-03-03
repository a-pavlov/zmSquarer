TEMPLATE = app

QT += qml quick multimedia widgets network

SOURCES += main.cpp

RESOURCES += qml.qrc images.qrc text.qrc

CONFIG(debug, debug|release): message(Project is built in DEBUG mode.)
CONFIG(release, debug|release): message(Project is built in RELEASE mode.)

# Additional import path used to resolve QML modules in Qt Creator's code model

QML_IMPORT_PATH += $$PWD/qml

TJPEG_ROOT = $$(TJPEG_ROOT)

!isEmpty(TJPEG_ROOT) {
    QMAKE_CXXFLAGS += -DWITH_TURBOJPEG
}

win32 {
    include(winconf.pri)
}

unix:!macx {
  include(unixconf.pri)
}

#include(src/motion/motion.pri)
include(src/http_parser/http_parser.pri)
include(src/src.pri)

HEADERS +=
