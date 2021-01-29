#ifndef COLORARRAYTEST_H
#define COLORARRAYTEST_H

#include <QObject>

class ColorArrayTest : public QObject
{
    Q_OBJECT
public:
    explicit ColorArrayTest(QObject *parent = nullptr);

private slots:
    void initialTest();
};

#endif // COLORARRAYTEST_H
