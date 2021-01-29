QT += testlib network

SOURCES = main.cpp \
    test.cpp \
    ../src/rbuffer.cpp \
    ../src/rsplitter.cpp \
    ../src/zmdata.cpp \
    httpparsertest.cpp \
    zmclienttest.cpp \
    colorarraytest.cpp \
    ../src/colormatrix.cpp

INCLUDEPATH += ../src


include(../src/http_parser/http_parser.pri)

HEADERS += \
    test.h \
    ../src/rbuffer.h \
    ../src/rsplitter.h \
    ../src/zmdata.h \
    httpparsertest.h \
    zmclienttest.h \
    colorarraytest.h \
    ../src/colormatrix.h

RESOURCES += \
    res.qrc
