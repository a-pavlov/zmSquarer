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
    QObject(parent)
  , current_reply(nullptr) {

}

void ZMClient::getMonitors() {
    Q_ASSERT(!baseUrl.isEmpty());
    if (token.access_token.isEmpty() && !username.isEmpty() && !password.isEmpty()) {
        getLogin([this](){getMonitors();});
        return;
    }

    QNetworkRequest request(baseUrl + "/zm/api/monitors.json?" + getToken());
    QNetworkReply* reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->get(request);
    current_reply = reply;

    /*
    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [this](QNetworkReply::NetworkError code) {
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
    });*/

    QObject::connect(reply, &QNetworkReply::sslErrors, [reply](const QList<QSslError> &errors) {
        QList<QSslError> canBeIgnoredErrors;
        canBeIgnoredErrors << QSslError::HostNameMismatch << QSslError::SelfSignedCertificate;
        for (const QSslError& sslError: errors) {
            if (!canBeIgnoredErrors.contains(sslError.error())) return;
        }

        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, this, [reply, this]() {
        //ZMAPIRequest* originator = dynamic_cast<ZMAPIRequest*>(reply->request().originatingObject());
        //Q_ASSERT(originator != nullptr);
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray buffer = reply->readAll();
            QJsonParseError jsonParseError;
            QList<ZMMonitor> mons = ZMMonitor::fromJson(QJsonDocument::fromJson(buffer, &jsonParseError));
            if (jsonParseError.error == QJsonParseError::NoError) {
                //qDebug() << "monitors " << mons.size() << " data size bytes " << buffer.size();
                emit monitors(mons, mons.size());
            } else {
                emit error(tr("Can not parse server's JSON response: %1").arg(jsonParseError.errorString()));
            }
        } else if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
            emit authentificationRequired();
        } else {
            emit error(reply->errorString());
        }

        // prevent cancel call on already stopped requst
        current_reply = nullptr;
        reply->deleteLater();
    });
}

void ZMClient::getLogin(std::function<void()> callback) {
    Q_ASSERT(!baseUrl.isEmpty());

    auto reqUrl = QString{"/zm/api/host/login.json?user=%1&pass=%2"};
    QNetworkRequest request(baseUrl + reqUrl.arg(username, password));
    QByteArray data;
    QNetworkReply* reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->post(request, data);
    current_reply = reply;

    QObject::connect(reply, &QNetworkReply::sslErrors, [reply](const QList<QSslError> &errors) {
        QList<QSslError> canBeIgnoredErrors;
        canBeIgnoredErrors << QSslError::HostNameMismatch << QSslError::SelfSignedCertificate;
        for (const QSslError& sslError: errors) {
            if (!canBeIgnoredErrors.contains(sslError.error())) return;
        }

        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, this, [callback, reply, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray buffer = reply->readAll();
            QJsonParseError jsonParseError;
            token = ZMToken::fromJson(QJsonDocument::fromJson(buffer, &jsonParseError));
            if (jsonParseError.error == QJsonParseError::NoError) {
                callback();
            } else {
                emit error(tr("Can not parse server's JSON response: %1").arg(jsonParseError.errorString()));
            }
        } else if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
            username.clear();
            password.clear();
            emit invalidCredentials();
        } else {
            emit error(reply->errorString());
        }

        // prevent cancel call on already stopped requst
        current_reply = nullptr;
        reply->deleteLater();
    });
}

QString ZMClient::getToken() const {
    if (!token.access_token.isEmpty()) {
        return QString("token=") + token.access_token;
    }
    return QString();
}

void ZMClient::getVersion() {
    if (token.access_token.isEmpty() && !username.isEmpty() && !password.isEmpty()) {
        getLogin([this](){getVersion();});
        return;
    }

    auto reqUrl = QString{"/zm/api/host/getVersion.json?"} + getToken();
    QNetworkRequest request(baseUrl + reqUrl);
    QNetworkReply* reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->get(request);
    current_reply = reply;

    QObject::connect(reply, &QNetworkReply::sslErrors, [reply](const QList<QSslError> &errors) {
        QList<QSslError> canBeIgnoredErrors;
        canBeIgnoredErrors << QSslError::HostNameMismatch << QSslError::SelfSignedCertificate;
        for (const QSslError& sslError: errors) {
            if (!canBeIgnoredErrors.contains(sslError.error())) return;
        }

        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, this, [reply, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray buffer = reply->readAll();
            QJsonParseError jsonParseError;
            auto zmVersion = ZMVersion::fromJson(QJsonDocument::fromJson(buffer, &jsonParseError));
            if (jsonParseError.error == QJsonParseError::NoError) {
                emit version(zmVersion.version);
            } else {
                emit error(tr("Can not parse server's JSON response: %1").arg(jsonParseError.errorString()));
            }
        } else if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
            token.access_token.clear();
            emit authentificationRequired();
        } else {
            emit error(reply->errorString());
        }

        // prevent cancel call on already stopped requst
        current_reply = nullptr;
        reply->deleteLater();
    });
}

void ZMClient::setUrl(const QString &u) {
    baseUrl = u;
}

QString ZMClient::url() const {
    return baseUrl;
}

bool ZMClient::supportsSsl() const {
    return QSslSocket::supportsSsl();
}

QString ZMClient::getMonitorUrl(int monId) const {
    Q_ASSERT(monId > 0);
    Q_ASSERT(!baseUrl.isEmpty());
    return baseUrl + "/zm/cgi-bin/nph-zms?mode=jpeg&monitor=" + QString::number(monId) + "&scale=100&maxfps=30&buffer=1000&" + getToken();
}

void ZMClient::cancel() {
    if (current_reply != nullptr) {
        current_reply->abort();
    }
}

QString ZMClient::getMonitorUrl() const {
    QString strTkn = token.access_token.isEmpty()?QString():QString("&token=%1").arg(token.access_token);
    return baseUrl + "/zm/cgi-bin/nph-zms?mode=jpeg&monitor=%1&scale=100&maxfps=30&buffer=1000" + strTkn;
}

void ZMClient::setCredentials(const QString& u, const QString& p) {
    username = u;
    password = p;
    token.access_token.clear();
}

QString ZMClient::getUsername() const {
    return username;
}

QString ZMClient::getPassword() const {
    return password;
}


