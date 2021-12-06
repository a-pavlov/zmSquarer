#ifndef ZMSEARCH_H
#define ZMSEARCH_H

#include <QObject>
#include <QAbstractListModel>
#include <QHostAddress>
#include "zmdata.h"

class ZMSearch : public QAbstractListModel {
    Q_OBJECT
private:
    typedef QPair<QString, QPair<QString, QString> > ZMHost;
    void continueSearch();
    int requestsInProgress;
    quint32 totalRequests;
    QList<QHostAddress> pendingRequests;
    QList<QPair<QString, ZMVersion>> hosts;
    void startRequest(const QHostAddress&);
    quint32 seachListSize;
    QList<ZMHost> knownHosts;
    void addHost(const QString& ip, const QString& version, const QString& apiversion);
public:
    enum ServerRoles {
        AddressRole   = Qt::UserRole + 1,
        VersionRole,
        ApiVersionRole
    };

    static int MAX_SEARCH_REQ;
    static int SUBNET_SIZE;
    static void registerQmlType();
    ZMSearch(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void search(const QList<QString>& interfaces);
    Q_INVOKABLE void cancel();
    int getTotalRequests() const;
    int getCompletedRequests() const;

    Q_PROPERTY(bool inProgress READ getInProgress NOTIFY inProgressChanged)

    bool getInProgress() const;
signals:
    void found(QString);
    void inProgressChanged(bool);
};

#endif // ZMSEARCH_H
