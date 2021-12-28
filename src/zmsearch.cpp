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
    , requestsInProgress(0)
    , maxRequests(0)
    , completedRequests(0)
    , cancelRequested(false)
    , checkedHostIndex(-1)
    , httpsEnabled(false) {
    // test hosts
    //hosts.append(qMakePair(QString("192.168.11.22"), ZMVersion("1.2", "1.2")));
    //hosts.append(qMakePair(QString("192.168.11.23"), ZMVersion("1.1", "1.2")));
    //hosts.append(qMakePair(QString("192.168.1.12"), ZMVersion("1.0", "1.2")));
    //hosts.append(qMakePair(QString("192.168.1.26"), ZMVersion("1.4", "1.2")));
}

int ZMSearch::getTotalRequests() const {
    return pendingRequests.size() + requestsInProgress;
}

void ZMSearch::startRequest(const QHostAddress& address) {
    QString url = (httpsEnabled?"https://":"http://") + address.toString() + "/zm/api/host/getVersion.json";
    QNetworkRequest request(url);
    QNetworkReply* reply = dynamic_cast<ZMSQApplication*>(QApplication::instance())->getNetMan()->get(request);

    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [this](QNetworkReply::NetworkError code) {
        Q_UNUSED(this)
        Q_UNUSED(code)
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
                emit found(reply->url().scheme() + "://" + reply->request().url().toString());
                this->addHost(reply->url().scheme() + "://" + reply->url().host(), version);
            }
        }

        this->requestsInProgress--;
        this->completedRequests++;
        emit progressChanged(getProgress());
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
        if (requestsInProgress == 1) {
            emit inProgressChanged(true);
        }

        startRequest(pendingRequests.takeFirst());

    }

    if (requestsInProgress == 0) {
        emit inProgressChanged(false);
    }
}

void ZMSearch::search(const QList<QString>& addresses, bool https) {
    pendingRequests = ZMUtils::genTestHosts(addresses);
    maxRequests = pendingRequests.size();
    requestsInProgress = 0;
    completedRequests = 0;
    httpsEnabled = https;
    emit progressChanged(getProgress());

    if (!hosts.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, hosts.size() - 1);
        hosts.clear();
        endRemoveRows();
    }

    cancelRequested = false;
    // for now ignore next runs search the same again and add info multiple times
    if (requestsInProgress != 0) {
        // continue
        emit inProgressChanged(true);
    }

    continueSearch();
}

void ZMSearch::cancel() {
    cancelRequested = true;
    emit inProgressChanged(false);
    emit progressChanged(getProgress());
    pendingRequests.clear();
    maxRequests = 0;
}

void ZMSearch::addHost(const QString& ip, const ZMVersion& zmv) {
    beginInsertRows(QModelIndex(), hosts.size(), hosts.size());
    hosts.append(qMakePair(ip, zmv));
    endInsertRows();
}


QHash<int, QByteArray> ZMSearch::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[AddressRole]      = "ip";
    roles[VersionRole]      = "version";
    roles[ApiVersionRole]   = "apiversion";
    roles[CheckRole]        = "check";
    roles[UpdRole]          = "upd";
    return roles;
}

int ZMSearch::rowCount(const QModelIndex&) const {
    return hosts.size();
}

QVariant ZMSearch::data(const QModelIndex & index, int role) const {
    if (!index.isValid()) return QVariant();
    Q_ASSERT(index.row() < hosts.size());
    const ZMHost& zh = hosts.at(index.row());
    switch(role) {
        case Qt::DisplayRole:   return zh.first;
        case AddressRole:       return zh.first;
        case VersionRole:       return zh.second.version;
        case ApiVersionRole:    return zh.second.apiversion;
        case CheckRole:         return index.row() == checkedHostIndex;
    default:
        break;
    }

    return QVariant();
}

bool ZMSearch::setData(const QModelIndex& index, const QVariant &value, int role /*= Qt::EditRole*/) {
    Q_UNUSED(value)
    if (index.isValid() && (index.row() >= 0 && index.row() < rowCount() && index.column() >= 0)) {
        if (role == UpdRole) {
            if (checkedHostIndex == index.row()) {
                checkedHostIndex = -1;
                emit dataChanged(index, index);
                return true;
            }

            int prevCheckedIndex = checkedHostIndex;
            checkedHostIndex = index.row();

            if (prevCheckedIndex!= -1) {
                emit dataChanged(createIndex(prevCheckedIndex, 0), createIndex(prevCheckedIndex, 0));
            }

            emit dataChanged(createIndex(index.row(), 0), createIndex(index.row(), 0));
            return true;
        }
    }

    return false;
}

bool ZMSearch::getInProgress() const {
    bool res = !cancelRequested && requestsInProgress > 0;
    return res;
}

qreal ZMSearch::getProgress() const {
    return maxRequests!=0?1.0*completedRequests/maxRequests:0.0;
}

