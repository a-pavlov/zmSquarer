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
    res.colorIndex  = 0;
    return res;
}

ZMMonitor::ZMMonitor():
    id("1000")
    , name("n/a")
    , function("n/a")
    , host("n/a")
    , path("n/a")
    , size(QSize(0,0))
    , status("n/a")
    , captureFPS("n/a")
    , type(CamType::UNKNOWN)
    , visualIndex(0)
    , colorIndex(1000)
{
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

ZMMonitor& ZMMonitor::operator=(const ZMMonitor& zmmon) {
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
    return *this;
}

ZMMonitor::~ZMMonitor(){}

ZMVersion::ZMVersion(const QString& ver, const QString& aver) :
    version(ver), apiversion(aver) {

}

ZMVersion ZMVersion::fromJson(const QJsonDocument& doc) {
    ZMVersion zmversion;
    zmversion.version = doc.object().value("version").toString();
    zmversion.apiversion = doc.object().value("apiversion").toString();
    return zmversion;
}

ZMToken ZMToken::fromJson(const QJsonDocument& doc) {
    ZMToken tkn;
    tkn.access_token            = doc.object().value("access_token").toString();
    tkn.access_token_expires    = doc.object().value("access_token_expires").toInt();
    tkn.refresh_token           = doc.object().value("refresh_token").toString();
    tkn.refresh_token_expires   = doc.object().value("refresh_token_expires").toInt();
    tkn.credentials             = doc.object().value("credentials").toString();
    tkn.append_password         = doc.object().value("append_password").toString();
    tkn.version                 = doc.object().value("version").toString();
    tkn.apiversion              = doc.object().value("apiversion").toString();
    return tkn;
}
