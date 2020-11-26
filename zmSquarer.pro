TEMPLATE = app

QT += qml quick multimedia widgets

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
include(deployment.pri)
include(src/motion/motion.pri)

HEADERS += \
    DesktopVideoProducer.h
