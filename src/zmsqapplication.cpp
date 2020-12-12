#include "zmsqapplication.h"
#include <QThread>
#include <QDebug>
#include <QtGlobal>

#include "camcontroller.h"

ZMSQApplication::ZMSQApplication(int &argc, char **argv)
    : QApplication (argc, argv) {
    connect(this, &QCoreApplication::aboutToQuit, this, &ZMSQApplication::cleanup);
}

ZMSQApplication::~ZMSQApplication() {
    qDebug() << "application closed: " << thread()->currentThreadId();
}

int ZMSQApplication::exec() {
    return QApplication::exec();
}

void ZMSQApplication::cleanup() {
    qDebug() << "zmsqapp cleanup";
}
