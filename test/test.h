#ifndef RANGETEST_H
#define RANGETEST_H

#include <QObject>

class Test : public QObject
{
    Q_OBJECT
public:
    explicit Test(QObject *parent = nullptr);

private slots:
    void testAlgorithm();
    void testRBuffer();
    void testRBufferMemStep();
    void testRSplitterTrivial();
    void testRPSplitterNoMatchPartial();
    void testRSplitterMatchNotEnoughBufferNotMatchNext();
    void testRSplitterWholeMatchTrivial();
    void testRSplitterMultiplePatternMatches();
    void testRSplitterMatchBetweenBuffers();
};

#endif // RANGETEST_H
