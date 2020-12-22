#include "zmclienttest.h"
#include <QTest>
#include "zmdata.h"

ZMClientTest::ZMClientTest(QObject *parent)
    : QObject(parent) {

}

void ZMClientTest::testMonitors() {
    QString fileName(":/zmclient/monitors.json");
    QFile file(fileName);
    bool isOpened = file.open(QIODevice::ReadOnly);
    QVERIFY(isOpened);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    QList<ZMMonitor> monitors = ZMMonitor::fromJson(doc);
    QCOMPARE(monitors.size(), 15);
}
