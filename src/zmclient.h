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

class QNetworkReply;

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
    bool supportsSsl() const;
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool supportsSsl READ supportsSsl NOTIFY supportsSslChanged)
    Q_INVOKABLE QString getMonitors();
    Q_INVOKABLE QString getMonitorUrl(int monId) const;
    Q_INVOKABLE QString getMonitorUrl() const;
    Q_INVOKABLE void getLogin(const QString& login, const QString& password);
    Q_INVOKABLE void cancel();    
signals:
    void monitors(const QList<ZMMonitor>& mons, int mcount);
    void error(const QString& msg);
    void authentificationRequired();
    void urlChanged(const QString& url);
    void supportsSslChanged(bool);
    void logged();
public slots:
private:
    QString baseUrl;
    QNetworkReply* current_reply;
    ZMToken token;
};

#endif // ZMCLIENT_H
