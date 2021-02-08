#include "monitor_model.h"
#include <QDebug>
#include <QtQml/qqml.h>

#include "preferences.h"

void MonitorModel::registerQmlType() {
    qmlRegisterType<MonitorModel>(
        "MonitorModel", 0, 1, "MonitorModel" );
}

MonitorModel::MonitorModel(QObject* parent)
    : QAbstractListModel(parent) {
}

QHash<int, QByteArray> MonitorModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole]           = "mid";
    roles[NameRole]         = "name";
    roles[HostRole]         = "host";
    roles[PathRole]         = "path";
    roles[SizeRole]         = "size";
    roles[StatusRole]       = "monStatus";
    roles[CaptureFPSRole]   = "captureFPS";
    roles[CheckedRole]      = "selected";
    roles[TypeRole]         = "type";
    roles[ColorRole]        = "color";
    roles[ColorIndexRole]   = "colorIndex";
    roles[VisualIndexRole]  = "visualIndex";
    return roles;
}

int MonitorModel::rowCount(const QModelIndex&) const {
    return monitors.size();
}

QString camType2String(CamType ct) {
    switch (ct) {
        case CamType::CAM: return "cam";
        case CamType::NEW_LINE: return "newline";
        case CamType::END_SCREEN: return "stopper";
    }

    return "_|_";
}

QVariant MonitorModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();
    Q_ASSERT(index.row() < monitors.size());
    const ZMMonitor& mon = monitors.at(index.row());
    switch(role) {
        case Qt::DisplayRole:   return mon.name;
        case IdRole:            return mon.id;
        case NameRole:          return mon.name;
        case HostRole:          return mon.host;
        case PathRole:          return mon.path;
        case SizeRole:          return QString::number(mon.size.width()) + "x" + QString::number(mon.size.height());
        case StatusRole:        return mon.status;
        case CaptureFPSRole:    return mon.captureFPS;
        case CheckedRole:       return checked.at(index.row());
        case TypeRole:
            return camType2String(mon.type);
        case ColorIndexRole:
            return colorMatrix.findCamColorIndex(mon.colorIndex);
        case ColorRole:
            //qDebug() << "get color for index " << index.row() << " type " << camType2String(mon.type);
            return mon.type==CamType::CAM?colorMatrix.getColor(colorMatrix.findCamColorIndex(mon.colorIndex)):"lightgrey";
    default:
        break;
    }

    return QVariant();
}

bool MonitorModel::setData(const QModelIndex& index, const QVariant &value, int role/* = Qt::EditRole*/) {
    qDebug() << "set data override " << index.row() << " value " << value;
    if (index.isValid() && (index.row() >= 0 && index.row() < rowCount() && index.column() >= 0)) {
        if (role == CheckedRole) {
            checked[index.row()] = value.toBool();
            emit checkedCountChanged();
            return true;
        }

        if (role == ColorRole) {
            colorMatrix.nextColorIndex(monitors.at(index.row()).colorIndex, false);
            emit dataChanged(index, index);
            return true;
        }

        if (role == VisualIndexRole) {
            qDebug() << "set visual index " << value;
        }
    }

    return false;
}

QModelIndex MonitorModel::getIndex(const QString& id) const {
    for(int i = 0; i < monitors.size(); ++i) {
        if (monitors.at(i).id == id) return index(i);
    }

    return QModelIndex();
}

void MonitorModel::add(const ZMMonitor& mon) {
    Preferences pref;
    beginInsertRows(QModelIndex(), monitors.size(), monitors.size());
    monitors << mon;
    if (mon.type == CamType::CAM) {
        colorMatrix.addCam(colorMatrix.size());
        monitors.back().colorIndex = colorMatrix.size() - 1;
    }
    Q_ASSERT(static_cast<size_t>(monitors.size()) >= colorMatrix.size());
    while(checked.size() < monitors.size()) checked.append(pref.isCheckedMon(monitors[checked.size()].id));
    endInsertRows();
    emit dataIncoming(monitors.size());
    emit monitorsCountChanged();
}

void MonitorModel::addAll(const QList<ZMMonitor>& monitors) {
    foreach (const ZMMonitor& mon, monitors) {
        add(mon);
    }
}

void MonitorModel::testAdd() {
    ZMMonitor mon;
    mon.id = QString::number(monitors.size());
    mon.host = "192.168.100.11";
    mon.name = "Test " + QString::number(monitors.size());
    mon.type = CamType::CAM;
    mon.size = QSize(1920, 1080);
    mon.status = "Connected";
    add(mon);
}

void MonitorModel::addNewLine() {
    ZMMonitor mon;
    mon.type = CamType::NEW_LINE;
    mon.name = "NL";
    add(mon);
}

void MonitorModel::addStopper() {
    ZMMonitor mon;
    mon.type = CamType::END_SCREEN;
    mon.name = "END";
    add(mon);
}

void MonitorModel::remove(int index) {
    beginRemoveRows(QModelIndex(), index, index);
    qDebug() << "remove " << index;
    monitors.removeAt(index);
    endRemoveRows();
    emit monitorsCountChanged();
}

void MonitorModel::changeColor(int index) {
    //emit dataChanged(QModelIndex())
}

void MonitorModel::setVisualIndex(int modelIndex, int visualIndex) {
    Q_ASSERT(modelIndex < monitors.size());
    monitors[modelIndex].visualIndex = visualIndex;
}

void MonitorModel::clear() {
    if (!monitors.isEmpty()) {
        colorMatrix.reset();
        emit beginRemoveRows(QModelIndex(), 0, monitors.size() - 1);
        monitors.clear();
        endRemoveRows();
        emit monitorsCountChanged();
    }
}

void MonitorModel::onMonitors(const QList<ZMMonitor>& monitors) {
    foreach (const ZMMonitor& mon, monitors) {
        add(mon);
    }
}

QString MonitorModel::getCheckedMonId(int row) const {
    Q_ASSERT(row < rowCount());
    return checked.at(row)?monitors.at(row).id:QString("");
}

int MonitorModel::getCheckedCount() const {
    return static_cast<int>(std::count_if(checked.begin(), checked.end(), [](bool i) {return i; }));
}

int MonitorModel::getMonitorsCount() const {
    return static_cast<int>(std::count_if(monitors.begin(), monitors.end(), [](const ZMMonitor& m) -> bool { return m.type == CamType::CAM; }));
}

/*
void MonitorModel::update(const QString& alias, const QString& host, int port) {
    QModelIndex index = getIndex(alias, host, port);
    if (index.isValid()) {
        Q_ASSERT(index.row() < servers.size());
        switch(servers.at(index.row()).status) {
            case QED2KServer::ServerConnected:
            case QED2KServer::ServerConnecting:
                qDebug() << "disconnect " << alias;
                Session::instance()->stopServerConnection();
                break;
            case QED2KServer::ServerDisconnected:
                qDebug() << "connect " << alias;
                Session::instance()->startServerConnection(servers.at(index.row()));
                servers[index.row()].status = QED2KServer::ServerConnecting;
                emit dataChanged(index, index);
                break;
            default:
                break;
        }
    }
}

void MonitorModel::remove(const QString& alias, const QString& host, int port) {
    QModelIndex index = getIndex(alias, host, port);
    if (index.isValid()) {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        servers.removeAt(index.row());
        endRemoveRows();
    }
}
*/

/*
void MonitorModel::setClientId(const QString& alias, const QString& host, int port, quint32 client_id) {
    QModelIndex index = getIndex(alias, host, port);
    if (index.isValid()) {
        servers[index.row()].clientId = client_id;
        emit dataChanged(index, index);
    }
}
*/
/*
void MonitorModel::on_serverConnectionInitialized(QString alias, QString host, int port, quint32 client_id, quint32 tcp_flags, quint32 aux_port) {
    Q_UNUSED(client_id);
    Q_UNUSED(tcp_flags);
    Q_UNUSED(aux_port);
    QModelIndex index = getIndex(alias, host, port);
    if (index.isValid()) {
        Q_ASSERT(index.row() < servers.size());
        servers[index.row()].status = QED2KServer::ServerConnected;
        emit dataChanged(index, index);
    }
}

void MonitorModel::on_serverConnectionClosed(QString alias, QString host, int port, QString strError) {
    Q_UNUSED(strError);
    QModelIndex index = getIndex(alias, host, port);
    if (index.isValid()) {
        Q_ASSERT(index.row() < servers.size());
        servers[index.row()].status = QED2KServer::ServerDisconnected;
        emit dataChanged(index, index);
    }
}
*/
/*void MonitorModel::load() {
    Preferences pref;
    if (pref.getServersPresence()) {
        int size = pref.beginReadArray("Servers");
        for(int i = 0; i < size; ++i) {
            pref.setArrayIndex(i);
            add(load_server(pref));
        }

        pref.endArray();
    } else {
        add(QED2KServer("is74", "emule.is74.ru", 4661));
        add(QED2KServer("TV Underground", "176.103.48.36", 4184));
        add(QED2KServer("eMule Security No1", "91.200.42.46" ,1176));
        add(QED2KServer("eMule Security No2", "91.200.42.47", 3883));
        add(QED2KServer("eMule Security No3", "91.200.42.119", 9939));
        add(QED2KServer("eMule Security No4", "77.120.115.66", 5041));
        add(QED2KServer("eMule oVPN.to Anonymous ed2k-Server", "213.163.71.135", 4242));
        add(QED2KServer("emule lover QQ Qun 212431292", "27.152.28.252", 4242));
        add(QED2KServer("PEERATES.NET", "195.154.83.5" ,7111));
        add(QED2KServer("PeerBooter", "212.83.184.152", 7111));
        add(QED2KServer("!! www.Sharing-Devils.org No.1 ...", "195.154.109.229" ,4232));

        foreach(const QED2KServer s, fromServersMet("./server.met")) {
            add(s);
        }
    }

    qDebug() << "servers count " << servers.size();
}

void MonitorModel::save() const {
    Preferences pref;
    pref.setServersPresence(true);
    pref.beginWriteArray("Servers", servers.size());
    int index = 0;
    foreach(const QED2KServer& s, servers) {
        pref.setArrayIndex(index++);
        save_server(s, pref);
    }

    pref.endArray();

}
*/

void MonitorModel::clean() {
    if (!monitors.empty()) {
        beginRemoveRows(QModelIndex(), 0, monitors.size() - 1);
        monitors.clear();
        endRemoveRows();
        emit monitorsCountChanged();
    }
}

void MonitorModel::save() const {
    colorMatrix.save();
    Preferences pref;
    pref.beginWriteArray("Common/MonitorModel");
    for(int index = 0; index < monitors.size(); ++index) {
        pref.setArrayIndex(index);
        pref.setValue("Id", monitors[index].id);
        pref.setValue("Host", monitors[index].host);
        pref.setValue("Name", monitors[index].name);
        pref.setValue("Path", monitors[index].path);
        pref.setValue("Size", monitors[index].size);
        pref.setValue("Type", static_cast<int>(monitors[index].type));
        pref.setValue("Status", monitors[index].status);
        pref.setValue("Function", monitors[index].function);
        pref.setValue("CaptureFPS", monitors[index].captureFPS);
        pref.setValue("VisualIndex", monitors[index].visualIndex);
        pref.setValue("ColorIndex", monitors[index].colorIndex);
    }

    pref.endArray();
    pref.sync();
}

void MonitorModel::load() {
    Preferences pref;
    QList<ZMMonitor> mons;
    int count = pref.beginReadArray("Common/MonitorModel");
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
        mons.append(zm);
    }

    pref.endArray();

    std::sort(mons.begin(), mons.end(), [](const ZMMonitor& a, const ZMMonitor& b) -> bool { return a.visualIndex < b.visualIndex; });
    clear();
    colorMatrix.load();
    if (!mons.isEmpty()) {
        emit beginInsertRows(QModelIndex(), 0, mons.size() - 1);
        monitors << mons;
        emit endInsertRows();
    }

    monitorsCountChanged();
}
