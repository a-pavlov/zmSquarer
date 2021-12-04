#include "zmclienttest.h"
#include <QTest>
#include "zmdata.h"
#include "zmutils.h"

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
    QCOMPARE(monitors[0].id, "1");
    QCOMPARE(monitors[0].name, "Front");
    QCOMPARE(monitors[0].function, "Monitor");
    QCOMPARE(monitors[0].host, "http://192.168.100.6:8899/onvif/device_service");
    QCOMPARE(monitors[0].path, "rtsp://admin:admin@192.168.100.6:554/user=admin_password=6QNMIQGe_channel=1_stream=0.sdp?real_stream");
    QCOMPARE(monitors[0].size.width(), 1920);
    QCOMPARE(monitors[0].size.height(), 1080);
    QCOMPARE(monitors[0].status, "Connected");
    QCOMPARE(monitors[0].captureFPS, "12.50");
}

void ZMClientTest::testVersion() {
    QString fileName(":/zmclient/version.json");
    QFile file(fileName);
    bool isOpened = file.open(QIODevice::ReadOnly);
    QVERIFY(isOpened);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    ZMVersion version = ZMVersion::fromJson(doc);
    QCOMPARE(version.version, "1.34.21");
    QCOMPARE(version.apiversion, "2.0");
    QVERIFY(!version.isEmpty());
}

void ZMClientTest::testVersionBad() {
    QString fileName(":/zmclient/version_bad.json");
    QFile file(fileName);
    bool isOpened = file.open(QIODevice::ReadOnly);
    QVERIFY(isOpened);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    ZMVersion version = ZMVersion::fromJson(doc);
    QVERIFY(version.isEmpty());
}

void ZMClientTest::testSearch() {
    QList<QString> src;
    src << "192.168.100.13" << "10.2.0.1";
    QList<QHostAddress> res = ZMUtils::genTestHosts(src);
    QCOMPARE(res.size(), ZMUtils::SUB_NET_HOSTS_COUNT*2);
    QCOMPARE(res.at(0), QHostAddress("192.168.100.1"));
    QCOMPARE(res.at(252), QHostAddress("192.168.100.254"));
    QCOMPARE(res.at(253), QHostAddress("10.2.0.2"));
    QCOMPARE(res.at(253*2 - 1), QHostAddress("10.2.0.254"));

    QList<QString> src2;
    src2 << "192.168.1.20";
    QCOMPARE(ZMUtils::genTestHosts(src2).at(252), QHostAddress("192.168.1.254"));
}
