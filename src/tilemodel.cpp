#include "tilemodel.h"
#include <QDebug>
#include <QtQml/qqml.h>

void TileModel::registerQmlType() {
    qmlRegisterType<TileModel>(
        "TileModel", 0, 1, "TileModel");
}


TileModel::TileModel(QObject* parent/* = nullptr*/):
    QAbstractListModel(parent) {
    for(quint32 index = 0; index < CAM_LINE*CAM_LINE; ++index) {
        tiles.append(-1);
    }
}

QHash<int, QByteArray> TileModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[SelectorRole]     = "select";
    roles[PosIndexRole]     = "pos";
    roles[NameRole]         = "name";
    roles[HaveMonRole]      = "havemon";
    roles[UpdateRole]       = "upd";
    return roles;
}

int TileModel::rowCount(const QModelIndex& /*parent = QModelIndex()*/) const {
    return static_cast<int>(tiles.size());
}

QVariant TileModel::data(const QModelIndex & index, int role /* = Qt::DisplayRole*/) const {
    if (!index.isValid()) return QVariant();
    Q_ASSERT(index.row() < tiles.size());
    qint32 tIndex = tiles.at(index.row());
    switch(role) {
        case Qt::DisplayRole:   return (isValidMonIndex(tIndex)?availableMons.at(tIndex).name:tr("Off"));
        case NameRole:          return (isValidMonIndex(tIndex)?availableMons.at(tIndex).name:tr("Off"));
        case HaveMonRole:       return isValidMonIndex(tIndex);
        case SelectorRole:      return tIndex;
        case PosIndexRole:      return index.row();
    default:
        break;
    }

    return QVariant();
}

bool TileModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/) {
    if (index.isValid() && (index.row() >= 0 && index.row() < rowCount() && index.column() >= 0)) {
        if (role == UpdateRole) {
            qDebug() << "update was requested for " << index.row() << " payload " << value;
            ++tiles[index.row()];
            if (tiles[index.row()] == availableMons.size()) {
                tiles[index.row()] = -1;
            }

            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

void TileModel::setAvailableMons(const QList<ZMMonitor> &am) {
    beginResetModel();
    for(qint32 i = 0; i < tiles.size(); ++i) {
        if (tiles[i] >= am.size()) tiles[i] = -1;
    }

    availableMons = am;
    endResetModel();
}
