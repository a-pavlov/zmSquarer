QT += testlib network

SOURCES = main.cpp \
    test.cpp \
    ../src/rbuffer.cpp \
    ../src/rsplitter.cpp \
    httpparsertest.cpp

INCLUDEPATH += ../src


include(../src/http_parser/http_parser.pri)

HEADERS += \
    test.h \
    ../src/rbuffer.h \
    ../src/rsplitter.h \
    httpparsertest.h
