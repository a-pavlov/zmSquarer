TEMPLATE = app

QT += qml quick multimedia widgets network

SOURCES += main.cpp

RESOURCES += qml.qrc \
    image.qrc

QMAKE_CFLAGS += -Dsysconfdir=\"xxx\"
QMAKE_CXXFLAGS += -Dsysconfdir=\"xxx\" -DWITH_TURBOJPEG
#LIBS += -lpthread -ljpeg

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

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
