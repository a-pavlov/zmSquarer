#include <QCoreApplication>
#include <QtTest/QtTest>

#include "test.h"
#include "httpparsertest.h"
#include "zmclienttest.h"
#include "colorarraytest.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    Test test;
    HttpParserTest htest;
    ZMClientTest zmctest;
    ColorArrayTest caTest;
    return QTest::qExec(&test, argc, argv)
            | QTest::qExec(&htest, argc, argv)
            | QTest::qExec(&zmctest, argc, argv)
            | QTest::qExec(&caTest, argc, argv);
}
