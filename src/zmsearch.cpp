#include "zmsearch.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtQml/qqml.h>

#include "zmsqapplication.h"
#include "zmdata.h"
#include "zmutils.h"
#include <algorithm>

int ZMSearch::MAX_SEARCH_REQ = 10;
int ZMSearch::SUBNET_SIZE = 254;



void ZMSearch::registerQmlType() {
    qmlRegisterType<ZMSearch>(
        "ZMSearch", 0, 1, "ZMSearch" );
}

ZMSearch::ZMSearch(QObject* parent) :
    QAbstractListModel(parent)
    , requestsInProgress(0) {
}

int ZMSearch::getTotalRequests() const {
    return pendingRequests.size() + requestsInProgress;
}

int ZMSearch::getCompletedRequests() const {
    return totalRequests;
}

void ZMSearch::startRequest(const QHostAddress& address) {
    QString url = "https://" + address.toString() + "/zm/api/host/getVersion.json";
    qDebug() << "start request " << url;
    QNetworkRequest request(url);
    QNetworkReply* reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->get(request);

    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [this](QNetworkReply::NetworkError code) {
        Q_UNUSED(this)
        qDebug() << "network error occurred " << code;
        /*switch (code) {
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
        }*/
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, [reply](const QList<QSslError> &errors) {
        QList<QSslError> canBeIgnoredErrors;
        canBeIgnoredErrors << QSslError::HostNameMismatch << QSslError::SelfSignedCertificate;
        for (const QSslError& sslError: errors) {
            if (!canBeIgnoredErrors.contains(sslError.error())) return;
        }

        reply->ignoreSslErrors(errors);
    });

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray buffer = reply->readAll();
            ZMVersion version = ZMVersion::fromJson(QJsonDocument::fromJson(buffer));
            if (!version.isEmpty()) {
                qDebug() << "version " << version.version;
                emit found(reply->request().url().toString());
                hosts.append(qMakePair(reply->request().url().toString(), version));
                qDebug() << "found host " << reply->request().url();
            }
        }

        this->requestsInProgress--;
        reply->deleteLater();
        continueSearch();
    });
}

void ZMSearch::continueSearch() {
    while (!pendingRequests.isEmpty()) {
        if (requestsInProgress == MAX_SEARCH_REQ) {
            break;
        }

        ++requestsInProgress;
        startRequest(pendingRequests.takeFirst());
    }
}

void ZMSearch::search(const QList<QString>& addresses) {
    pendingRequests = ZMUtils::genTestHosts(addresses);
    totalRequests = pendingRequests.size();
    hosts.clear();
    requestsInProgress = 0;
    continueSearch();
}

void ZMSearch::cancel() {
    pendingRequests.clear();
}

int ZMSearch::rowCount(const QModelIndex & parent) const {
    return 0;
}

QVariant ZMSearch::data(const QModelIndex & index, int role) const {
    return QVariant();
}
