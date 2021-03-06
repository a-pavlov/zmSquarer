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
    default:
        break;
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
    //qDebug() << "set data override " << index.row() << " value " << value;
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
            //qDebug() << "set visual index " << value;
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
    monitors.removeAt(index);
    endRemoveRows();
    emit monitorsCountChanged();
}

void MonitorModel::setVisualIndex(int modelIndex, int visualIndex) {
    Q_ASSERT(modelIndex < monitors.size());
    monitors[modelIndex].visualIndex = visualIndex;
}

void MonitorModel::clear() {
    if (!monitors.isEmpty()) {
        Q_ASSERT(colorMatrix.size() > 0);
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
