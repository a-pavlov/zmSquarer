#include <QCoreApplication>
#include <QtTest/QtTest>

#include "test.h"
#include "httpparsertest.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);    
    Test test;
    HttpParserTest htest;
    return QTest::qExec(&test, argc, argv) | QTest::qExec(&htest, argc, argv);
}
