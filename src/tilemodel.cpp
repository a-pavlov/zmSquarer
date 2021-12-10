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
        tiles.append(qMakePair(-1, -1));
    }
}

QHash<int, QByteArray> TileModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[SelectorRole]     = "select";
    roles[PosIndexRole]     = "pos";
    roles[NameRole]         = "name";
    roles[HRNameRole]       = "hrname";
    roles[HaveMonRole]      = "havemon";
    roles[HaveHRRole]       = "havehr";
    roles[UpdateRole]       = "upd";
    roles[UpdateHiResRole]  = "updhr";
    return roles;
}

int TileModel::rowCount(const QModelIndex& /*parent = QModelIndex()*/) const {
    return static_cast<int>(tiles.size());
}

QVariant TileModel::data(const QModelIndex & index, int role /* = Qt::DisplayRole*/) const {
    if (!index.isValid()) return QVariant();
    Q_ASSERT(index.row() < tiles.size());
    auto tIndex = tiles.at(index.row());
    switch(role) {
        case Qt::DisplayRole:   return (isValidMonIndex(tIndex.first)?availableMons.at(tIndex.first).name:tr("Off"));
        case NameRole:          return (isValidMonIndex(tIndex.first)?availableMons.at(tIndex.first).name:tr("Off"));
        case HRNameRole:        return (isValidMonIndex(tIndex.second)?availableMons.at(tIndex.second).name:tr("Off"));
        case HaveMonRole:       return isValidMonIndex(tIndex.first);
        case HaveHRRole:        return isValidMonIndex(tIndex.second);
        case SelectorRole:      return tIndex.first;
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
            ++(tiles[index.row()].first);
            if (tiles[index.row()].first == availableMons.size()) {
                tiles[index.row()].first = -1;
                tiles[index.row()].second = -1;
            }

            emit dataChanged(index, index);
            return true;
        }

        if (role == UpdateHiResRole) {
            qDebug() << "update hi res";
            ++(tiles[index.row()].second);
            if (tiles[index.row()].second == availableMons.size()) {
                tiles[index.row()].second = -1;
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
        if (tiles[i].first >= am.size()) tiles[i].first = -1;
        if (tiles[i].second >= am.size()) tiles[i].second = -1;
    }

    availableMons = am;
    endResetModel();
}
