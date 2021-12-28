#include "tilemodel.h"
#include <QDebug>
#include <QtQml/qqml.h>
#include "preferences.h"

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
    roles[IdRole]           = "id";
    roles[StatusRole]       = "status";
    roles[ResolutionRole]   = "resolution";
    roles[NameRoleHR]       = "name_hr";
    roles[IdRoleHR]         = "id_hr";
    roles[StatusRoleHR]     = "status_hr";
    roles[ResolutionRoleHR] = "resolution_hr";
    roles[HaveMonRole]      = "havemon";
    roles[HaveMonRoleHR]    = "havemon_hr";
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
        case Qt::DisplayRole:   return (isValidMonIndex(tIndex.first)?availableMons.at(tIndex.first).name:QString());
        case NameRole:          return (isValidMonIndex(tIndex.first)?availableMons.at(tIndex.first).name:QString());
        case IdRole:            return (isValidMonIndex(tIndex.first)?availableMons.at(tIndex.first).id:QString());
        case StatusRole:        return (isValidMonIndex(tIndex.first)?availableMons.at(tIndex.first).status:QString());
        case ResolutionRole:    return (isValidMonIndex(tIndex.first)?(QString::number(availableMons.at(tIndex.first).size.height()) + "x" + QString::number(availableMons.at(tIndex.first).size.width())):QString());
        case NameRoleHR:        return (isValidMonIndex(tIndex.second)?availableMons.at(tIndex.second).name:QString());
        case IdRoleHR:          return (isValidMonIndex(tIndex.second)?availableMons.at(tIndex.second).id:QString());
        case StatusRoleHR:      return (isValidMonIndex(tIndex.second)?availableMons.at(tIndex.second).status:QString());
        case ResolutionRoleHR:  return (isValidMonIndex(tIndex.second)?(QString::number(availableMons.at(tIndex.second).size.height()) + "x" + QString::number(availableMons.at(tIndex.second).size.width())):QString());
        case HaveMonRole:       return isValidMonIndex(tIndex.first);
        case HaveMonRoleHR:     return isValidMonIndex(tIndex.second);
        case SelectorRole:      return tIndex.first;
        case PosIndexRole:      return index.row();
    default:
        break;
    }

    return QVariant();
}

bool TileModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/) {
    Q_UNUSED(value)
    if (index.isValid() && (index.row() >= 0 && index.row() < rowCount() && index.column() >= 0)) {
        if (role == UpdateRole) {
            ++(tiles[index.row()].first);
            if (tiles[index.row()].first == availableMons.size()) {
                tiles[index.row()].first = -1;
                tiles[index.row()].second = -1;
            }

            emit dataChanged(index, index);
            emit numeratedTilesChanged();
            return true;
        }

        if (role == UpdateHiResRole) {
            ++(tiles[index.row()].second);
            if (tiles[index.row()].second == availableMons.size()) {
                tiles[index.row()].second = -1;
            }

            emit dataChanged(index, index);
            emit numeratedTilesChanged();
            return true;
        }
    }

    return false;
}

void TileModel::setAvailableMons(const QList<ZMMonitor> &am, const QString url) {
    beginResetModel();
    zmBaseUrl = url;
    for(qint32 i = 0; i < tiles.size(); ++i) {
        if (tiles[i].first >= am.size()) tiles[i].first = -1;
        if (tiles[i].second >= am.size()) tiles[i].second = -1;
    }

    availableMons = am;
    endResetModel();
}

QList<QList<TileModel::TILE_NUM> > TileModel::getNumeratedTiles() const {
    QList<QList<TILE_NUM>> result;
    qint32 index = 0;
    for(qint32 row = 0; row < CAM_LINE; ++row) {
        QList<TILE_NUM> line;
        for(qint32 i = row*CAM_LINE; i < row*CAM_LINE + 4; ++i) {
            if (tiles.at(i).first != -1) {
                auto resMonId = availableMons.at(tiles.at(i).first).id.toInt();
                auto resHiMonId = (tiles.at(i).second != -1)?availableMons.at(tiles.at(i).second).id.toInt():-1;
                line.append(qMakePair(++index, qMakePair(resMonId, resHiMonId)));
            }
        }

        if (!line.isEmpty()) {
            result.append(line);
        }
    }

    return result;
}

qint32 TileModel::getNumeratedTilesCount() const {
    return std::count_if(tiles.begin(), tiles.end(),
                         [](const TileModel::TILE& t) { return t.first != -1;});
}

void TileModel::load() {
    Preferences pref;
    int count = pref.beginReadArray("Common/AvailableMonitors");
    for(int index = 0; index < count; ++index) {
        pref.setArrayIndex(index);
        ZMMonitor zm;
        zm.id       = pref.value("Id").toString();
        zm.host     = pref.value("Host").toString();
        zm.name     = pref.value("Name").toString();
        zm.size     = pref.value("Size").toSize();
        zm.path     = pref.value("Path").toString();
        zm.type     = static_cast<CamType>(pref.value("Type").toInt());
        zm.status   = pref.value("Status").toString();
        zm.function = pref.value("Function").toString();
        zm.captureFPS   = pref.value("CaptureFPS").toString();
        zm.visualIndex  = pref.value("VisualIndex").toInt();
        zm.colorIndex   = pref.value("ColorIndex").toUInt();
        availableMons.append(zm);
    }

    pref.endArray();
    zmBaseUrl = pref.value("Common/TileModelBaseUrl").toString();

    count = pref.beginReadArray("Common/TileModel");
    if (count != tiles.size()) return;
    for(int index = 0; index < count; ++index) {
        pref.setArrayIndex(index);
        tiles[index] = qMakePair(static_cast<qint32>(pref.value("Id").toInt()), static_cast<qint32>(pref.value("HiResId").toInt()));
        dataChanged(createIndex(index, 0), createIndex(index, 0));
        emit numeratedTilesChanged();
    }

    pref.endArray();
}

void TileModel::save() const {
    Preferences pref;
    pref.beginWriteArray("Common/AvailableMonitors");
    for(int index = 0; index < availableMons.size(); ++index) {
        pref.setArrayIndex(index);
        pref.setValue("Id", availableMons[index].id);
        pref.setValue("Host", availableMons[index].host);
        pref.setValue("Name", availableMons[index].name);
        pref.setValue("Path", availableMons[index].path);
        pref.setValue("Size", availableMons[index].size);
        pref.setValue("Type", static_cast<int>(availableMons[index].type));
        pref.setValue("Status", availableMons[index].status);
        pref.setValue("Function", availableMons[index].function);
        pref.setValue("CaptureFPS", availableMons[index].captureFPS);
        pref.setValue("VisualIndex", availableMons[index].visualIndex);
        pref.setValue("ColorIndex", availableMons[index].colorIndex);
    }

    pref.endArray();

    pref.setValue("Common/TileModelBaseUrl", zmBaseUrl);

    pref.beginWriteArray("Common/TileModel");
    for(int index = 0; index < tiles.size(); ++index) {
        pref.setArrayIndex(index);
        pref.setValue("Id", tiles[index].first);
        pref.setValue("HiResId", tiles[index].second);
    }

    pref.endArray();
    pref.sync();
}
