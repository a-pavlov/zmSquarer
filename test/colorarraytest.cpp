#include "colorarraytest.h"]
#include "colormatrix.h"
#include <QTest>

ColorArrayTest::ColorArrayTest(QObject *parent)
    : QObject(parent) {
}

void ColorArrayTest::initialTest() {
    ColorMatrix cm(10);
    for(size_t i = 0; i < 10; ++i)
        QCOMPARE(cm.findCamColorIndex(i), CC_MAX);

    QCOMPARE(cm.nextColorIndex(0, true), 0u);
    QCOMPARE(cm.nextColorIndex(1, true), 1u);
    QCOMPARE(cm.nextColorIndex(2, true), 2u);
    QCOMPARE(cm.nextColorIndex(9, true), 3u);
    QCOMPARE(cm.nextColorIndex(8, true), 4u);
    QCOMPARE(cm.nextColorIndex(7, true), 5u);
    QCOMPARE(cm.nextColorIndex(6, true), 6u);
    QCOMPARE(cm.nextColorIndex(5, true), 7u);
    QCOMPARE(cm.nextColorIndex(4, true), 8u);
    QCOMPARE(cm.nextColorIndex(3, true), 9u);

    QCOMPARE(cm.findCamColorIndex(0u), 0u);
    QCOMPARE(cm.findCamColorIndex(1u), 1u);
    QCOMPARE(cm.findCamColorIndex(2u), 2u);
    QCOMPARE(cm.findCamColorIndex(3u), 9u);
    QCOMPARE(cm.findCamColorIndex(4u), 8u);
    QCOMPARE(cm.findCamColorIndex(5u), 7u);
    QCOMPARE(cm.findCamColorIndex(6u), 6u);
    QCOMPARE(cm.findCamColorIndex(7u), 5u);
    QCOMPARE(cm.findCamColorIndex(8u), 4u);
    QCOMPARE(cm.findCamColorIndex(9u), 3u);

    // must not use in code, but still
    QCOMPARE(cm.nextColorIndex(0u, true), 0u);
    QCOMPARE(cm.nextColorIndex(1u, true), 1u);
    QCOMPARE(cm.nextColorIndex(2u, true), 2u);
    QCOMPARE(cm.nextColorIndex(3u, true), 9u);
    QCOMPARE(cm.nextColorIndex(4u, true), 8u);
    QCOMPARE(cm.nextColorIndex(5u, true), 7u);
    QCOMPARE(cm.nextColorIndex(6u, true), 6u);
    QCOMPARE(cm.nextColorIndex(7u, true), 5u);
    QCOMPARE(cm.nextColorIndex(8u, true), 4u);
    QCOMPARE(cm.nextColorIndex(9u, true), 3u);

    // combine colors
    QCOMPARE(cm.nextColorIndex(0u, false), 1u);
    QCOMPARE(cm.findCamColorIndex(1u), 1u);
    QCOMPARE(cm.nextColorIndex(2u, false), 3u);
    QCOMPARE(cm.findCamColorIndex(9u), 3u);
    QCOMPARE(cm.nextColorIndex(4u, false), 9u);
    QCOMPARE(cm.findCamColorIndex(3u), 9u);
    QCOMPARE(cm.nextColorIndex(5u, false), 8u);
    QCOMPARE(cm.nextColorIndex(6u, false), 7u);
    QCOMPARE(cm.nextColorIndex(7u, false), 6u);
    QCOMPARE(cm.nextColorIndex(8u, false), 5u);
    QCOMPARE(cm.nextColorIndex(9u, false), 4u);

    QCOMPARE(cm.nextColorIndex(9u, false), 5u);
    QCOMPARE(cm.nextColorIndex(7u, false), 7u);
    QCOMPARE(cm.findCamColorIndex(6u), 7u);
    QCOMPARE(cm.findCamColorIndex(8u), 5u);

    QCOMPARE(cm.nextColorIndex(4u, false), 0u);
    QCOMPARE(cm.nextColorIndex(4u, false), 2u);
    QCOMPARE(cm.nextColorIndex(4u, false), 3u);
    QCOMPARE(cm.nextColorIndex(4u, false), 4u);
    QCOMPARE(cm.nextColorIndex(4u, false), 6u);
    QCOMPARE(cm.nextColorIndex(4u, false), 8u);
    QCOMPARE(cm.findCamColorIndex(5u), 8u);
}
