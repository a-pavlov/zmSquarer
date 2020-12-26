#include "monitor_model.h"
#include <QDebug>
#include <QtQml/qqml.h>

void MonitorModel::registerQmlType() {
    qmlRegisterType<MonitorModel>(
        "MonitorModel", 0, 1, "MonitorModel" );
}

MonitorModel::MonitorModel(QObject* parent)
    : QAbstractListModel(parent) {}

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
    return roles;
}

int MonitorModel::rowCount(const QModelIndex&) const {
    return monitors.size();
}

QVariant MonitorModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();
    Q_ASSERT(index.row() < monitors.size());
    Q_ASSERT(monitors.size() == checked.size());
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
    default:
        break;
    }

    return QVariant();
}

bool MonitorModel::setData(const QModelIndex& index, const QVariant &value, int role/* = Qt::EditRole*/) {
    if (index.isValid() && (index.row() >= 0 && index.row() < rowCount() && index.column() >= 0)) {
        if (role == CheckedRole) {
            checked[index.row()] = value.toBool();
            return true;
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
    /*foreach(const QED2KServer& srv, servers) {
        if (srv == s) {
            return;
        }
    }*/

    beginInsertRows(QModelIndex(), monitors.size(), monitors.size());
    qDebug() << "add " << mon.name;
    monitors << mon;
    while(checked.size() < monitors.size()) checked.append(false);
    endInsertRows();
    emit dataIncoming(monitors.size());
}

void MonitorModel::addAll(const QList<ZMMonitor>& monitors) {
    foreach (const ZMMonitor& mon, monitors) {
        add(mon);
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
    }
}
