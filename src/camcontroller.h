#ifndef CAMCONTROLLER_H
#define CAMCONTROLLER_H

#include <QObject>
#include <QThread>

class NetCam;

class CamController : public QObject {
    Q_OBJECT
public:
    explicit CamController(QObject *parent = nullptr);
    ~CamController();
    NetCam* startCam(const QString&);
private:
    QThread networkThread;
public slots:
};

#endif // CAMCONTROLLER_H
