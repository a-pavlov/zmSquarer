#include "networksmodel.h"
#include <QtQml/qqml.h>
#include <QHostAddress>
#include <QNetworkInterface>


NetworksModel::NetworksModel(QObject* parent)
    : QAbstractListModel(parent)
{
}


void NetworksModel::registerQmlType() {
    qmlRegisterType<NetworksModel>(
        "NetworksModel", 0, 1, "NetworksModel" );
}


QHash<int, QByteArray> NetworksModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[AddressRole] = "address";
    return roles;
};

int NetworksModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent)
    return addresses.size();
}

QVariant NetworksModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid()) return QVariant();
    Q_ASSERT(index.row() < addresses.size());
    const QString& address = addresses.at(index.row());
    switch(role) {
        case Qt::DisplayRole:   return address;
        case AddressRole:       return address;
    default:
        break;
    }

    return QVariant();

}

void NetworksModel::refresh() {
    if (!addresses.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, addresses.size() - 1);
        addresses.clear();
        endRemoveRows();
    }

    QList<QHostAddress> ifaces = QNetworkInterface::allAddresses();

    for(QList<QHostAddress>::iterator itr = ifaces.begin(); itr != ifaces.end(); ++itr) {
        if (itr->protocol() ==  QAbstractSocket::IPv4Protocol && !itr->isLoopback()) {
            beginInsertRows(QModelIndex(), addresses.size(), addresses.size());
            addresses.append(itr->toString());
            endInsertRows();
            //qDebug() << (itr->isLoopback()?"loop":"global")
            //     << " GLOBAL " << (itr->isGlobal()?"YES":"NO")
            //     << " IP: " << itr->toIPv4Address()
            //     << " PROTO: " << itr->protocol()
            //     << " STR: " << itr->toString();
        }
    }
}
