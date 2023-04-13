#ifndef ZMDATA_H
#define ZMDATA_H

#include <cstdint>
#include <QObject>
#include <QJsonDocument>
#include <QSize>

enum class CamType: std::int8_t { CAM = 1, NEW_LINE = 2, END_SCREEN = 3, UNKNOWN = 4 };

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
      ZMMonitor& operator=(const ZMMonitor&);
      ~ZMMonitor();
      static QList<ZMMonitor> fromJson(const QJsonDocument&);
      static ZMMonitor fromJson(const QJsonObject&);
};

struct ZMVersion {
    QString version;
    QString apiversion;
    ZMVersion(const QString& ver, const QString& aver);
    ZMVersion() = default;
    ZMVersion(const ZMVersion&) = default;
    ZMVersion& operator=(const ZMVersion&) = default;
    bool isEmpty() const {
        return version.isEmpty() && apiversion.isEmpty();
    }
    static ZMVersion fromJson(const QJsonDocument&);
};

struct ZMToken {
    QString access_token;
    uint access_token_expires;
    QString refresh_token;
    uint refresh_token_expires;
    QString credentials;
    QString append_password;
    QString version;
    QString apiversion;
    static ZMToken fromJson(const QJsonDocument&);
};

Q_DECLARE_METATYPE(ZMMonitor)

#endif // ZMDATA_H
