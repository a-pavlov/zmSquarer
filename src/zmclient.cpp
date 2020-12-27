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

QString ZMClient::getMonitors() {
    Q_ASSERT(!url.isEmpty());
    QNetworkRequest request(url + "/zm/api/monitors.json");
    //request.setOriginatingObject(new ZMAPIRequest());
    QNetworkReply* reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->get(request);

    /*QObject::connect(reply, &QIODevice::readyRead, [reply]() {
        Q_UNUSED(reply);
        //ZMAPIRequest* originator = dynamic_cast<ZMAPIRequest*>(reply->request().originatingObject());
        //Q_ASSERT(originator != nullptr);
        //originator->buffer.append(reply->readAll());
        // do nothing here
    });*/

    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [this](QNetworkReply::NetworkError code) {
        qDebug() << "network error occurred " << code;
        switch (code) {
            case QNetworkReply::NetworkError::ConnectionRefusedError:       emit error(QString("Connection refused")); break;
            case QNetworkReply::NetworkError::RemoteHostClosedError:        emit error(QString("Remote host closed")); break;
            case QNetworkReply::NetworkError::HostNotFoundError:            emit error(QString("Host not found")); break;
            case QNetworkReply::NetworkError::TimeoutError:                 emit error(QString("Timeout")); break;
            case QNetworkReply::NetworkError::OperationCanceledError:       emit error(QString("Operation canceled")); break;
            case QNetworkReply::NetworkError::SslHandshakeFailedError:      emit error(QString("Ssl handshake failed")); break;
            case QNetworkReply::NetworkError::TemporaryNetworkFailureError: emit error(QString("Temporary network failure")); break;
            case QNetworkReply::NetworkError::NetworkSessionFailedError:    emit error(QString("Network session failed")); break;
            case QNetworkReply::NetworkError::BackgroundRequestNotAllowedError: emit error(QString("Background request not allowed")); break;
            case QNetworkReply::NetworkError::TooManyRedirectsError:        emit error(QString("Too many redirects")); break;
            case QNetworkReply::NetworkError::InsecureRedirectError:        emit error(QString("Insecure redirect")); break;
            default:
                emit error(QString::number(code));
        }
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, [reply](const QList<QSslError> &errors) {
        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]() {
        //ZMAPIRequest* originator = dynamic_cast<ZMAPIRequest*>(reply->request().originatingObject());
        //Q_ASSERT(originator != nullptr);
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray buffer = reply->readAll();
            QList<ZMMonitor> mons = ZMMonitor::fromJson(QJsonDocument::fromJson(buffer));
            qDebug() << "monitors " << mons.size() << " data size bytes " << buffer.size();
            emit monitors(mons);
        }

        reply->deleteLater();
    });

    return QString();
}

void ZMClient::setUrl(const QString &u) {
    url = u;
}

QString ZMClient::getMonitorUrl(int monId) const {
    Q_ASSERT(monId > 0);
    Q_ASSERT(!url.isEmpty());
    return url + "/zm/cgi-bin/nph-zms?mode=jpeg&monitor=" + QString::number(monId) + "&scale=100&maxfps=30&buffer=1000&user=admin&pass=root";
}


