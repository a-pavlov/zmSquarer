#include <QtNetwork>
#include <QDebug>
#include <QtQml/qqml.h>
#include <QByteArray>
#include <QTimer>
#include "zmclient.h"
#include "zmsqapplication.h"


void closeReply(QNetworkReply* reply) {
    auto zmc = static_cast<ZMClient*>(reply->request().originatingObject());
    reply->deleteLater();
    zmc->reply = nullptr;
}

using unique_reply_t = std::unique_ptr<QNetworkReply, decltype(&closeReply)>;

QMutex ZMClient::mutex;
QString ZMClient::baseUrl;
QString ZMClient::tokenQueryParam;
ZMToken ZMClient::token;
QString ZMClient::username;
QString ZMClient::password;

QString ZMClient::getMonitorUrl(int monId) {
    Q_ASSERT(monId > 0);
    Q_ASSERT(!baseUrl.isEmpty());
    QMutexLocker locker(&mutex);
    return baseUrl + "/zm/cgi-bin/nph-zms?mode=jpeg&monitor=" + QString::number(monId) + "&scale=100&maxfps=30&buffer=1000&" + tokenQueryParam;
}

void ZMClient::setToken(ZMToken && t) {
    QMutexLocker locker(&mutex);
    token = t;
    if (!token.access_token.isEmpty()) {
        tokenQueryParam = QString("token=") + token.access_token;
    } else {
        tokenQueryParam.clear();
    }
}

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
  , reply(nullptr)
  , requestAborted(false) {
    tokenRefreshTimer = new QTimer(this);
    connect(tokenRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshToken()));
}

void ZMClient::getMonitors() {
    Q_ASSERT(!baseUrl.isEmpty());
    Q_ASSERT(reply == nullptr);
    if (token.access_token.isEmpty() && !username.isEmpty() && !password.isEmpty()) {
        getLogin([this](){getMonitors();});
        return;
    }

    QNetworkRequest request(baseUrl + "/zm/api/monitors.json?" + tokenQueryParam);
    request.setOriginatingObject(this);
    requestAborted = false;
    reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->get(request);

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

    QObject::connect(reply, &QNetworkReply::sslErrors, this, [this](const QList<QSslError> &errors) {
        QList<QSslError> canBeIgnoredErrors;
        canBeIgnoredErrors << QSslError::HostNameMismatch << QSslError::SelfSignedCertificate;
        for (const QSslError& sslError: errors) {
            if (!canBeIgnoredErrors.contains(sslError.error())) return;
        }

        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, this, [this]() {
        unique_reply_t ur(reply, &closeReply);

        if (requestAborted) {
            return;
        }


        if (reply->error() == QNetworkReply::NoError) {
            QByteArray buffer = reply->readAll();
            QJsonParseError jsonParseError;
            QList<ZMMonitor> mons = ZMMonitor::fromJson(QJsonDocument::fromJson(buffer, &jsonParseError));
            if (jsonParseError.error == QJsonParseError::NoError) {                
                emit monitors(mons, mons.size());
            } else {
                emit error(tr("Can not parse server's JSON response: %1").arg(jsonParseError.errorString()));
            }
        } else if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
            emit authentificationRequired();
        } else {
            emit error(reply->errorString());
        }
    });
}

void ZMClient::getLogin(std::function<void()> callback) {
    Q_ASSERT(!baseUrl.isEmpty());

    auto reqUrl = QString{"/zm/api/host/login.json?user=%1&pass=%2"};
    QNetworkRequest request(baseUrl + reqUrl.arg(username, password));
    request.setOriginatingObject(this);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    QByteArray data;
    requestAborted = false;
    reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->post(request, data);

    QObject::connect(reply, &QNetworkReply::sslErrors, this, [this](const QList<QSslError> &errors) {
        QList<QSslError> canBeIgnoredErrors;
        canBeIgnoredErrors << QSslError::HostNameMismatch << QSslError::SelfSignedCertificate;
        for (const QSslError& sslError: errors) {
            if (!canBeIgnoredErrors.contains(sslError.error())) return;
        }

        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, this, [callback, this]() {
        unique_reply_t ur(reply, &closeReply);

        if (requestAborted) {
            return;
        }

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray buffer = reply->readAll();
            QJsonParseError jsonParseError;
            token = ZMToken::fromJson(QJsonDocument::fromJson(buffer, &jsonParseError));
            if (token.access_token_expires > 10) {
                tokenRefreshTimer->start((token.access_token_expires - 10)*1000);
            }

            setToken(std::move(token));
            if (jsonParseError.error == QJsonParseError::NoError) {
                {
                    unique_reply_t early = std::move(ur);
                }
                if (callback != nullptr) {
                    callback();
                }
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
    });
}

void ZMClient::getVersion() {
    if (token.access_token.isEmpty() && !username.isEmpty() && !password.isEmpty()) {
        getLogin([this](){getVersion();});
        return;
    }

    auto reqUrl = QString{"/zm/api/host/getVersion.json?"} + tokenQueryParam;
    QNetworkRequest request(baseUrl + reqUrl);
    request.setOriginatingObject(this);
    requestAborted = false;
    reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->get(request);    

    QObject::connect(reply, &QNetworkReply::sslErrors, this, [this](const QList<QSslError> &errors) {
        QList<QSslError> canBeIgnoredErrors;
        canBeIgnoredErrors << QSslError::HostNameMismatch << QSslError::SelfSignedCertificate;
        for (const QSslError& sslError: errors) {
            if (!canBeIgnoredErrors.contains(sslError.error())) return;
        }

        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, this, [this]() {
        unique_reply_t ur(reply, &closeReply);

        if (requestAborted) {
            return;
        }

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

void ZMClient::cancel() {
    if (reply != nullptr) {
        requestAborted = true;
        reply->abort();
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


void ZMClient::refreshToken() {
    getLogin(nullptr);
}

