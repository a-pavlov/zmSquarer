#include "camcontroller.h"
#include "netcam2.h"

#include <QDebug>

CamController::CamController(QObject *parent) : QObject(parent) {
    networkThread.start();
}

CamController::~CamController() {
    qDebug() << "cam controller stop";
    networkThread.quit();
    networkThread.wait();
}

NetCam* CamController::createCam(int url) {
    NetCam* cam = new NetCam(url);
    cam->moveToThread(&networkThread);
    connect(&networkThread, &QThread::finished, cam, &QObject::deleteLater);
    //connect(this, &CamController::startCam, cam, &NetCam::start);
    //QMetaObject::invokeMethod(cam, "start", Qt::QueuedConnection);
    //Q_ARG(QString, url));
    return cam;
}

void CamController::startCam(NetCam* nc) {
    Q_ASSERT(nc != nullptr);
    QMetaObject::invokeMethod(nc, "start", Qt::QueuedConnection);
}
