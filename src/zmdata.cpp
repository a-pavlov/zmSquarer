#include "zmdata.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValueRef>
#include <QDebug>

QList<ZMMonitor> ZMMonitor::fromJson(const QJsonDocument& doc) {
    QList<ZMMonitor> res;
    if (doc.isNull()) {
        return res;
    }

    QJsonArray jsonArray = doc.object()["monitors"].toArray();
    qDebug() << "array size " << jsonArray.size();
    for(QJsonArray::const_iterator itr = jsonArray.constBegin(); itr != jsonArray.end(); ++itr) {
        if (itr->isObject()) {
            res.append(ZMMonitor::fromJson(itr->toObject()));
        }
    }

    return res;
}

ZMMonitor ZMMonitor::fromJson(const QJsonObject& doc) {
    ZMMonitor res;
    QJsonObject mon = doc["Monitor"].toObject();
    res.id          = mon.value("Id").toString();
    res.name        = mon.value("Name").toString();
    res.function    = mon.value("Function").toString();
    res.host        = mon.value("Host").toString();
    res.path        = mon.value("Path").toString();
    res.size        = QSize(mon.value("Width").toString().toInt(), mon.value("Height").toString().toInt());
    QJsonObject monStatus = doc["Monitor_Status"].toObject();
    res.status      = monStatus.value("Status").toString();
    res.captureFPS  = monStatus.value("CaptureFPS").toString();
    res.type        = CamType::CAM;
    res.visualIndex = 0;
    res.colorIndex  = -1;
    return res;
}

ZMMonitor::ZMMonitor() {

}

ZMMonitor::ZMMonitor(const ZMMonitor& zmmon) {
    id          = zmmon.id;
    name        = zmmon.name;
    function    = zmmon.function;
    host        = zmmon.host;
    path        = zmmon.path;
    name        = zmmon.name;
    size        = zmmon.size;
    status      = zmmon.status;
    captureFPS  = zmmon.captureFPS;
    type        = zmmon.type;
    visualIndex = zmmon.visualIndex;
    colorIndex  = zmmon.colorIndex;
}

ZMMonitor::~ZMMonitor(){}
