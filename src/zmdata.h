#ifndef ZMDATA_H
#define ZMDATA_H

#include <QObject>
#include <QJsonDocument>
#include <QSize>

struct ZMMonitor {
      QString id;
      QString name;
      QString function;
      QString host;
      QString path;
      QSize size;
      QString status;
      QString captureFPS;
      ZMMonitor();
      ZMMonitor(const ZMMonitor&);
      ~ZMMonitor();
      static QList<ZMMonitor> fromJson(const QJsonDocument&);
      static ZMMonitor fromJson(const QJsonObject&);
};

Q_DECLARE_METATYPE(ZMMonitor);

#endif // ZMDATA_H
