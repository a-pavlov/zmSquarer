QT += testlib

SOURCES = main.cpp \
    test.cpp \
    ../src/rbuffer.cpp \
    ../src/rsplitter.cpp

INCLUDEPATH += ../src


HEADERS += \
    test.h \
    ../src/rbuffer.h \
    ../src/rsplitter.h
