#ifndef ZMDATA_H
#define ZMDATA_H

#include <QObject>
#include <QJsonDocument>

struct ZMMonitor {
      QString id;
      QString name;
      QString function;
      QString host;
      QString path;
      qint32 width;
      qint32 height;
      static QList<ZMMonitor> fromJson(const QJsonDocument&);
      static ZMMonitor fromJson(const QJsonObject&);
};

#endif // ZMDATA_H
