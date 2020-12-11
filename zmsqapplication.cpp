#include "zmsqapplication.h"
#include <QThread>
#include <QDebug>
#include <QtGlobal>

#include "camcontroller.h"

ZMSQApplication::ZMSQApplication(int &argc, char **argv, QThread* p, CamController* cc)
    : QApplication (argc, argv)
    , networkThread(p)
    , camCtrl(cc) {
    connect(this, &QCoreApplication::aboutToQuit, this, &ZMSQApplication::cleanup);
}

ZMSQApplication::~ZMSQApplication() {
    qDebug() << "application closed: " << thread()->currentThreadId();
}

int ZMSQApplication::exec() {
    return QApplication::exec();
}

void ZMSQApplication::cleanup() {
    if (camCtrl != nullptr) {
        delete camCtrl;
    }

    if (networkThread != nullptr) {
        qDebug() << "application cleanup thread";
        networkThread->quit();
        networkThread->wait();
        networkThread->deleteLater();
    }
}

NetCam* ZMSQApplication::startNetCam(const QString& url) {
    Q_ASSERT(camCtrl != nullptr);
    QString ret;
    //QMetaObject::invokeMethod(camCtrl, "startCam", Qt::QueuedConnection, Q_ARG(QString, url));
    camCtrl->startCam(url);
    return camCtrl->getNetCam();
}
