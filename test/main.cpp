#include <QCoreApplication>
#include <QtTest/QtTest>

#include "test.h"

int main(int argc, char *argv[]) {
    //freopen("testing.log", "w", stdout);
    QCoreApplication app(argc, argv);    
    Test test;

    return QTest::qExec(&test, argc, argv);
            //| QTest::qExec(&rangeTest, argc, argv);
}


