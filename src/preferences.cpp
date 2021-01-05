#include "preferences.h"

#include <QDebug>
#include <QtQml/qqml.h>

const QString pathUrl = "Network/Url";
const QString pathMons = "Common/Monitors";


void Preferences::registerQmlType() {
    qmlRegisterType<Preferences>(
        "ZMSQPreferences", 0, 1, "ZMSQPreferences" );
}

Preferences::Preferences()
    : QIniSettings("ZM",  "squarer") {
    int size = beginReadArray(pathMons);
    for (int i = 0; i < size; ++i) {
        setArrayIndex(i);
        checkedMons.append(value("monId").toString());
        qDebug() << "append " << value("monId").toString();
    }
    endArray();
}

void Preferences::flush() {
    beginWriteArray(pathMons);
    for(int i = 0; i < checkedMons.size(); ++i) {
        setArrayIndex(i);
        setValue("monId", checkedMons.at(i));
        qDebug() << "set mon " << checkedMons.at(i);
    }

    endArray();
    sync();
    qDebug() << "sync prefs";
}

bool Preferences::isCheckedMon(const QString& id) {
    return checkedMons.contains(id);
}

void Preferences::setCheckedMon(const QString& id) {
    if (!checkedMons.contains(id)) checkedMons.append(id);
}

void Preferences::unsetCheckedMon(const QString& id) {
    checkedMons.removeAll(id);
}

QString Preferences::url() const {
    return value(pathUrl, "").toString();
}

void Preferences::setUrl(const QString &url) {
    setValue(pathUrl, url);
    emit urlChanged(url);
}

int Preferences::checkedMonsCount() const {
    return checkedMons.size();
}
