#ifndef ZMCLIENTTEST_H
#define ZMCLIENTTEST_H

#include <QObject>

class ZMClientTest : public QObject
{
    Q_OBJECT
public:
    explicit ZMClientTest(QObject *parent = nullptr);

private slots:
    void testMonitors();
};

#endif // ZMCLIENTTEST_H
