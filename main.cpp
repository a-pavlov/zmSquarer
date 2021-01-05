#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QMetaType>

#include "zmsqapplication.h"
#include "camvideoproducer.h"
#include "zmclient.h"
#include "monitor_model.h"
#include "zmdata.h"
#include "preferences.h"


int main(int argc, char *argv[]) {
    CamVideoProducer::registerQmlType();

    ZMClient::registerMetaType();
    ZMClient::registerQmlType();
    MonitorModel::registerQmlType();
    Preferences::registerQmlType();

    ZMSQApplication app(argc, argv);
    //MonitorModel monmod;
    qDebug() << "app thread id " << app.thread()->currentThreadId();
    QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    //engine.rootContext()->setContextProperty("monmod", &monmod);
    engine.load(QUrl(QStringLiteral("qrc:///zmSquarer.qml")));
    return app.exec();
}
