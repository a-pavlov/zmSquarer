#include "networksmodel.h"
#include <QtQml/qqml.h>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDebug>


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
    roles[SelectedMode] = "selected";
    return roles;
};

int NetworksModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent)
    return addresses.size();
}

QVariant NetworksModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid()) return QVariant();
    Q_ASSERT(index.row() < addresses.size());
    auto address = addresses.at(index.row());
    switch(role) {
        case Qt::DisplayRole:   return address.second;
        case AddressRole:       return address.second;
        case SelectedMode:      return address.first;
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
            addresses.append(qMakePair(true, itr->toString()));
            endInsertRows();
        }
    }

    emit selectedChanged(getSelectedCount());
}

bool NetworksModel::setData(const QModelIndex& index, const QVariant &value, int role/* = Qt::EditRole*/) {
    if (index.isValid() && (index.row() >= 0 && index.row() < rowCount() && index.column() >= 0)) {
        if (role == SelectedMode) {
            addresses[index.row()].first = value.toBool();
            emit selectedChanged(getSelectedCount());
            dataChanged(index, index);
            return true;
        }
    }

    return false;
}

int NetworksModel::getSelectedCount() const {
    return static_cast<int>(std::count_if(addresses.begin(), addresses.end(), [](const QPair<bool, QString>& a) { return a.first?1:0;}));
}

QList<QString> NetworksModel::getSelected() const {
    QList<QString> res;
    for(QList<QPair<bool, QString>>::const_iterator itr = addresses.begin(); itr != addresses.end(); ++itr) {
        if (itr->first) res.append(itr->second);
    }

    return res;
}
