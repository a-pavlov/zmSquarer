#ifndef ZMDATA_H
#define ZMDATA_H

#include <cstdint>
#include <QObject>
#include <QJsonDocument>
#include <QSize>

enum class CamType: std::int8_t { CAM = 1, NEW_LINE = 2, END_SCREEN = 3 };

struct ZMMonitor {
      QString id;
      QString name;
      QString function;
      QString host;
      QString path;
      QSize size;
      QString status;
      QString captureFPS;
      CamType type;
      int visualIndex;
      uint colorIndex;
      ZMMonitor();
      ZMMonitor(const ZMMonitor&);
      ~ZMMonitor();
      static QList<ZMMonitor> fromJson(const QJsonDocument&);
      static ZMMonitor fromJson(const QJsonObject&);
};

Q_DECLARE_METATYPE(ZMMonitor);

#endif // ZMDATA_H
