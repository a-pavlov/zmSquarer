#include <QQmlApplicationEngine>
#include <QDebug>

#include "zmsqapplication.h"
#include "camvideoproducer.h"


int main(int argc, char *argv[]) {
    CamVideoProducer::registerQmlType();    
    ZMSQApplication app(argc, argv);
    qDebug() << "app thread id " << app.thread()->currentThreadId();
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    return app.exec();
}
