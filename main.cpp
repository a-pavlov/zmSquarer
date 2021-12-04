#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QMetaType>

#include <QHostAddress>
#include <QNetworkInterface>

#include "zmsqapplication.h"
#include "camvideoproducer.h"
#include "zmclient.h"
#include "monitor_model.h"
#include "zmdata.h"
#include "preferences.h"
#include "scenebuilder.h"
#include "networksmodel.h"
#include "zmsearch.h"

#ifdef Q_OS_WIN

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);
    QString txt;
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type)
    {
        case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;

        case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
        case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
        case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
        default:
        abort();

    }

    QFile outFile("debuglog.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

#endif


int main(int argc, char *argv[]) {
#ifdef Q_OS_WIN
    //qInstallMessageHandler(customMessageHandler);
#endif
    qDebug() << "zmSquarer started";
    CamVideoProducer::registerQmlType();
    ZMClient::registerMetaType();
    ZMClient::registerQmlType();
    MonitorModel::registerQmlType();
    Preferences::registerQmlType();
    SceneBuilder::registerQmlType();
    NetworksModel::registerQmlType();
    ZMSearch::registerQmlType();


    ZMSQApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    //engine.load(QUrl(QStringLiteral("qrc:///zmSquarer2.qml")));
    return app.exec();
}
