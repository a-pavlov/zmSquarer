#include <QQmlApplicationEngine>
#include <QDebug>

#include "zmsqapplication.h"
#include "camvideoproducer.h"
#include "zmclient.h"


int main(int argc, char *argv[]) {
    CamVideoProducer::registerQmlType();
    ZMClient::registerQmlType();
    ZMSQApplication app(argc, argv);
    qDebug() << "app thread id " << app.thread()->currentThreadId();
    QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    engine.load(QUrl(QStringLiteral("qrc:///zmSquarer.qml")));
    return app.exec();
}
