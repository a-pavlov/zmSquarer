QT += testlib network

SOURCES = main.cpp \
    test.cpp \
    ../src/rbuffer.cpp \
    ../src/rsplitter.cpp \
    ../src/zmdata.cpp \
    httpparsertest.cpp \
    zmclienttest.cpp

INCLUDEPATH += ../src


include(../src/http_parser/http_parser.pri)

HEADERS += \
    test.h \
    ../src/rbuffer.h \
    ../src/rsplitter.h \
    ../src/zmdata.h \
    httpparsertest.h \
    zmclienttest.h

RESOURCES += \
    res.qrc
