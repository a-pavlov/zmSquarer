#ifndef ZMCLIENT_H
#define ZMCLIENT_H

#include <QObject>


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
    static void registerQmlType();
    explicit ZMClient(QObject *parent = nullptr);
    Q_INVOKABLE QString getMonitors(const QString& url);

signals:

public slots:
};

#endif // ZMCLIENT_H