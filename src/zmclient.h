#ifndef ZMCLIENT_H
#define ZMCLIENT_H

#include <QObject>
#include <QMutex>
#include <functional>
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
class QTimer;

/**
 * @brief The ZMClient class - simple one call in the same time client
 *
 */
class ZMClient : public QObject {
    Q_OBJECT
private:
    static QMutex mutex;
    static QString baseUrl;
    static QString tokenQueryParam;
    static ZMToken token;
    static QString username;
    static QString password;
public:
    static void registerMetaType();
    static void registerQmlType();
    static QString getMonitorUrl(int monId);
    static void setToken(ZMToken&& token);

    explicit ZMClient(QObject *parent = nullptr);
    void setUrl(const QString& url);
    QString url() const;
    bool supportsSsl() const;
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool supportsSsl READ supportsSsl NOTIFY supportsSslChanged)
    Q_PROPERTY(QString username READ getUsername)
    Q_PROPERTY(QString password READ getPassword)
    Q_INVOKABLE void getMonitors();
    Q_INVOKABLE void getVersion();
    Q_INVOKABLE QString getMonitorUrl() const;
    Q_INVOKABLE void setCredentials(const QString& login, const QString& password);
    Q_INVOKABLE void cancel();
signals:
    void monitors(const QList<ZMMonitor>& mons, int mcount);
    void version(const QString& ver);
    void error(const QString& msg);  
    void authentificationRequired();
    void invalidCredentials();
    void urlChanged(const QString& url);
    void supportsSslChanged(bool);
    void logged();
public slots:
private slots:
    void refreshToken();
private:
    void getLogin(std::function<void()>);
    QString getUsername() const;
    QString getPassword() const;    
    QNetworkReply* reply;
    bool requestAborted;
    friend void closeReply(QNetworkReply*);
    QTimer* tokenRefreshTimer;
};

#endif // ZMCLIENT_H
