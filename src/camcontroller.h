#ifndef CAMCONTROLLER_H
#define CAMCONTROLLER_H

#include <QObject>

class NetCam;

class CamController : public QObject {
    Q_OBJECT
public:
    explicit CamController(QObject *parent = nullptr);
    NetCam* getNetCam();
signals:


private:
    NetCam* pn;

public slots:
    void startCam(const QString&);
};

#endif // CAMCONTROLLER_H
