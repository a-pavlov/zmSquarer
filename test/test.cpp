#include "test.h"
#include <QTest>
#include <QList>
#include <QUrl>

#include "string.h"

#include "rbuffer.h"
#include "rsplitter.h"

Test::Test(QObject *parent) : QObject(parent) {
}

void Test::testUrl() {
    QUrl url("http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=1&scale=100&maxfps=30&buffer=1000&user=admin&pass=root");
    QCOMPARE(url.host(), QString("192.168.100.12"));
    QCOMPARE(url.port(), -1);
    QCOMPARE(url.path(), "/zm/cgi-bin/nph-zms");
    QCOMPARE(url.query(), "mode=jpeg&monitor=1&scale=100&maxfps=30&buffer=1000&user=admin&pass=root");
    QCOMPARE(url.scheme(), "http");
}

void Test::testAlgorithm() {
    QList<int> list;
    QVERIFY(qLowerBound(list.begin(), list.end(), 5) == list.end());
    QVERIFY(qLowerBound(list.begin(), list.end(), 5) == list.begin());
    list << 3 << 6 << 10;
    QVERIFY(qLowerBound(list.begin(), list.end(), 5) != list.begin());
    QList<int>::iterator itr = qLowerBound(list.begin(), list.end(), 1);
    QVERIFY(itr == list.begin());
    QList<int>::iterator itr2 = qLowerBound(list.begin(), list.end(), 4);
    QVERIFY(itr2 != list.begin());
    QList<int>::iterator itr3 = qLowerBound(list.begin(), list.end(), 12);
    QVERIFY(itr3 == list.end());
}

void Test::testRBuffer() {
    RBuffer rBuffer;
    RBuffer::RBUF_MEM_STEP = RBuffer::NETCAM_RBUFFSIZE;
    rBuffer.checkBufferSize(100);
    QCOMPARE(rBuffer.getSize(), RBuffer::NETCAM_RBUFFSIZE);
    rBuffer.checkBufferSize(1000);
    QCOMPARE(rBuffer.getSize(), RBuffer::NETCAM_RBUFFSIZE);
    rBuffer.checkBufferSize(100);
    QCOMPARE(rBuffer.getSize(), RBuffer::NETCAM_RBUFFSIZE);
    rBuffer.checkBufferSize(RBuffer::NETCAM_RBUFFSIZE + 1);
    QCOMPARE(rBuffer.getSize(), RBuffer::NETCAM_RBUFFSIZE*2);
    char data[] = {'\x12', '\x12'};
    rBuffer.append(data, 2);
    QCOMPARE(rBuffer.getUsed(), 2);
    QCOMPARE(rBuffer.getContentLength(), 0);
}

void Test::testRBufferMemStep() {
    RBuffer rBuffer;
    RBuffer::RBUF_MEM_STEP = 100;
    rBuffer.checkBufferSize(100);
    QCOMPARE(rBuffer.getSize(), 100);
    RBuffer rBuffer2;
    RBuffer::RBUF_MEM_STEP = 98;

}

void Test::testRSplitterTrivial() {
    RSplitter rsplitter(std::string("test"));
    RBuffer::RBUF_MEM_STEP = RBuffer::NETCAM_RBUFFSIZE;
    QCOMPARE(std::string("test"), std::string(rsplitter.getPattern()));
    char data[] = "xxxxxxxxx";
    rsplitter.read(data, strlen(data));
    QSharedPointer<RBuffer> crb = rsplitter.getCurrentRBuffer();
    QCOMPARE(crb->getSize(), RBuffer::NETCAM_RBUFFSIZE);
    QCOMPARE(crb->getUsed(), strlen(data));
    QCOMPARE(crb->getContentLength(), strlen(data));
    rsplitter.read(data, strlen(data));
    QCOMPARE(crb->getSize(), RBuffer::NETCAM_RBUFFSIZE);
    QCOMPARE(crb->getUsed(), strlen(data)*2);
    QCOMPARE(crb->getContentLength(), strlen(data)*2);
}

void Test::testRPSplitterNoMatchPartial() {
    RSplitter rsplitter(std::string("[ZM]"));
    RBuffer::RBUF_MEM_STEP = RBuffer::NETCAM_RBUFFSIZE;
    char data[] = "some information here [ZMI]";
    rsplitter.read(data, strlen(data));
    QSharedPointer<RBuffer> crb = rsplitter.getCurrentRBuffer();
    QCOMPARE(crb->getSize(), RBuffer::NETCAM_RBUFFSIZE);
    QCOMPARE(crb->getUsed(), strlen(data));
}

void Test::testRSplitterMatchNotEnoughBufferNotMatchNext() {
    RSplitter rsplitter(std::string("[ZM]"));
    RBuffer::RBUF_MEM_STEP = RBuffer::NETCAM_RBUFFSIZE;
    char data[] = "some information here [ZM";
    rsplitter.read(data, strlen(data));
    QSharedPointer<RBuffer> crb = rsplitter.getCurrentRBuffer();
    QCOMPARE(crb->getSize(), RBuffer::NETCAM_RBUFFSIZE);
    QCOMPARE(crb->getUsed(), strlen(data));
    QCOMPARE(crb->getContentLength(), strlen(data) - 3);
    rsplitter.read(data, strlen(data));
    QCOMPARE(crb->getSize(), RBuffer::NETCAM_RBUFFSIZE);
    QCOMPARE(crb->getUsed(), strlen(data)*2);
    QCOMPARE(crb->getContentLength(), strlen(data)*2 - 3);
}

void Test::testRSplitterWholeMatchTrivial() {
    RSplitter rsplitter(std::string("[ZM]"));
    RBuffer::RBUF_MEM_STEP = RBuffer::NETCAM_RBUFFSIZE;
    char data[] = "info[ZM]";
    QVERIFY(rsplitter.getLatestRBuffer().isNull());
    rsplitter.read(data, strlen(data));
    QSharedPointer<RBuffer> crb = rsplitter.getCurrentRBuffer();
    QSharedPointer<RBuffer> lrb = rsplitter.getLatestRBuffer();
    QVERIFY(!crb.isNull());
    QVERIFY(!lrb.isNull());
    QCOMPARE(lrb->getContentLength(), 4);
    QCOMPARE(lrb->getUsed(), 8);
    QCOMPARE(crb->getContentLength(), 0);
    QCOMPARE(crb->getUsed(), 0);
}

void Test::testRSplitterMultiplePatternMatches() {
    RSplitter rsplitter(std::string("[ZM]"));
    RBuffer::RBUF_MEM_STEP = RBuffer::NETCAM_RBUFFSIZE;
    char data[] = "info_1[ZM]info_2[ZM]info_3[ZM]info_4_bf";
    rsplitter.read(data, strlen(data));
    QSharedPointer<RBuffer> crb = rsplitter.getCurrentRBuffer();
    QSharedPointer<RBuffer> lrb = rsplitter.getLatestRBuffer();
    QVERIFY(!crb.isNull());
    QVERIFY(!lrb.isNull());
    QCOMPARE(crb->getContentLength(), 9);
    QCOMPARE(lrb->getContentLength(), 6);
    QCOMPARE(strncmp("info_4_bf", crb->getPtr(), crb->getContentLength()), 0);
    QCOMPARE(strncmp("info_3", lrb->getPtr(), lrb->getContentLength()), 0);
}

void Test::testRSplitterMatchBetweenBuffers() {
    RSplitter rsplitter(std::string("[ZM]"));
    RBuffer::RBUF_MEM_STEP = RBuffer::NETCAM_RBUFFSIZE;
    char data1[] = "info_1[";
    char data2[] = "ZM]XXXX";
    rsplitter.read(data1, strlen(data1));
    rsplitter.read(data2, strlen(data2));
    QSharedPointer<RBuffer> crb = rsplitter.getCurrentRBuffer();
    QSharedPointer<RBuffer> lrb = rsplitter.getLatestRBuffer();
    QVERIFY(!crb.isNull());
    QVERIFY(!lrb.isNull());
    QCOMPARE(crb->getContentLength(), 4);
    QCOMPARE(lrb->getContentLength(), 6);
    QCOMPARE(strncmp("XXXX", crb->getPtr(), crb->getContentLength()), 0);
    QCOMPARE(strncmp("info_1", lrb->getPtr(), lrb->getContentLength()), 0);
}

void Test::testRSplitterMatchNewIface() {
    RSplitter rsplitter(std::string(""));
    RBuffer::RBUF_MEM_STEP = 10;
    rsplitter.setPattern("[ZM]");
    char data1[] = "quite long data buffer[ZM]test data remain";
    QSharedPointer<RBuffer> cb = rsplitter.getCurrentRBuffer();
    char* ptr = cb->checkBufferSize(strlen(data1));
    memcpy(ptr, data1, strlen(data1));
    cb->expandUsed(strlen(data1));
    QCOMPARE(cb->getUnmarkedSize(), strlen(data1));
    rsplitter.processCB(cb);
    QSharedPointer<RBuffer> lrb = rsplitter.getLatestRBuffer();
    QSharedPointer<RBuffer> crb = rsplitter.getCurrentRBuffer();
    QVERIFY(!crb.isNull());
    QVERIFY(!lrb.isNull());
    QCOMPARE(lrb->getContentLength(), strlen("quite long data buffer"));
    QCOMPARE(crb->getContentLength(), strlen("test data remain"));
}
