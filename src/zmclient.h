#ifndef ZMCLIENT_H
#define ZMCLIENT_H

#include <QObject>
#include "zmdata.h"


/**
 * @brief The ZMAPIRequest class - unused for now
 */
class ZMAPIRequest : public QObject {
    Q_OBJECT
public:
    ZMAPIRequest();
    QByteArray buffer;
};

/**
 * @brief The ZMClient class - simple one call in the same time client
 *
 */
class ZMClient : public QObject {
    Q_OBJECT
public:
    static void registerMetaType();
    static void registerQmlType();
    explicit ZMClient(QObject *parent = nullptr);
    void setUrl(const QString& url);
    QString url() const;
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_INVOKABLE QString getMonitors();
    Q_INVOKABLE QString getMonitorUrl(int monId) const;
signals:
    void monitors(const QList<ZMMonitor>& mons);
    void error(const QString& msg);
    void urlChanged(const QString& url);

public slots:
private:
    QString baseUrl;
};

#endif // ZMCLIENT_H
