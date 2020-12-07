TEMPLATE = app

QT += qml quick multimedia widgets network

SOURCES += main.cpp \
    DesktopVideoProducer.cpp

RESOURCES += qml.qrc \
    image.qrc

QMAKE_CFLAGS += -Dsysconfdir=\"xxx\"
QMAKE_CXXFLAGS += -Dsysconfdir=\"xxx\"
LIBS += -lpthread -ljpeg

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(src/motion/motion.pri)
include(src/http_parser/http_parser.pri)
include(src/src.pri)

HEADERS += \
    DesktopVideoProducer.h
