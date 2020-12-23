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
            res.append(ZMMonitor::fromJson(itr->toObject()["Monitor"].toObject()));
        }
    }

    return res;
}

ZMMonitor ZMMonitor::fromJson(const QJsonObject& doc) {
    ZMMonitor res;
    res.id = doc.value("Id").toString();
    res.name = doc.value("Name").toString();
    res.function = doc.value("Function").toString();
    res.host = doc.value("Host").toString();
    res.path = doc.value("Path").toString();
    res.width = doc.value("Width").toString().toInt();
    res.height = doc.value("Height").toString().toInt();
    return res;
}
