#ifndef HTTPPARSERTEST_H
#define HTTPPARSERTEST_H

#include <QObject>

class HttpParserTest : public QObject
{
    Q_OBJECT
public:
    explicit HttpParserTest(QObject *parent = nullptr);

private slots:
    void test();
};

#endif // HTTPPARSERTEST_H
