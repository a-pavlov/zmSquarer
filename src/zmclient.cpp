#include "zmclient.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QtQml/qqml.h>
#include <QByteArray>
#include "zmsqapplication.h"


ZMAPIRequest::ZMAPIRequest() {}

void ZMClient::registerMetaType() {
    qRegisterMetaType<QList<ZMMonitor> >("<QList<ZMMonitor>>");
}

void ZMClient::registerQmlType() {
    qmlRegisterType<ZMClient>(
        "ZMClient", 0, 1, "ZMClient" );
}

ZMClient::ZMClient(QObject *parent) :
    QObject(parent) {

}

QString ZMClient::getMonitors(const QString &url) {
    QNetworkRequest request(url);
    //request.setOriginatingObject(new ZMAPIRequest());
    QNetworkReply* reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->get(request);

    /*QObject::connect(reply, &QIODevice::readyRead, [reply]() {
        Q_UNUSED(reply);
        //ZMAPIRequest* originator = dynamic_cast<ZMAPIRequest*>(reply->request().originatingObject());
        //Q_ASSERT(originator != nullptr);
        //originator->buffer.append(reply->readAll());
        // do nothing here
    });*/

    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [](QNetworkReply::NetworkError code) {
        qDebug() << "network error occurred " << code;
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, [reply](const QList<QSslError> &errors) {
        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]() {
        //ZMAPIRequest* originator = dynamic_cast<ZMAPIRequest*>(reply->request().originatingObject());
        //Q_ASSERT(originator != nullptr);
        QByteArray buffer = reply->readAll();
        QList<ZMMonitor> mons = ZMMonitor::fromJson(QJsonDocument::fromJson(buffer));
        qDebug() << "monitors " << mons.size() << " data size bytes " << buffer.size();
        emit monitors(mons);
        reply->deleteLater();
    });

    return QString();
}



