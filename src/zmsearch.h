#ifndef ZMSEARCH_H
#define ZMSEARCH_H

#include <QObject>
#include <QAbstractListModel>
#include <QHostAddress>
#include "zmdata.h"

class ZMSearch : public QAbstractListModel {
    Q_OBJECT
private:
    typedef QPair<QString, ZMVersion> ZMHost;
    void continueSearch();

    qint32 requestsInProgress;
    qint32 maxRequests;
    qint32 completedRequests;

    QList<QHostAddress> pendingRequests;
    QList<ZMHost> hosts;
    void startRequest(const QHostAddress&);
    quint32 seachListSize;
    void addHost(const QString& ip, const ZMVersion&);
    bool cancelRequested;
    int checkedHostIndex;
    bool httpsEnabled;
public:
    enum ServerRoles {
        AddressRole   = Qt::UserRole + 1,
        VersionRole,
        ApiVersionRole,
        CheckRole,
        UpdRole
    };

    static int MAX_SEARCH_REQ;
    static int SUBNET_SIZE;
    static void registerQmlType();
    ZMSearch(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant &value, int role = Qt::EditRole) override;
    Q_INVOKABLE void search(const QList<QString>& interfaces, bool https);
    Q_INVOKABLE void cancel();
    int getTotalRequests() const;

    Q_PROPERTY(bool inProgress READ getInProgress NOTIFY inProgressChanged)
    Q_PROPERTY(qreal progress READ getProgress NOTIFY progressChanged)

    bool getInProgress() const;
    qreal getProgress() const;
signals:
    void found(QString);
    void inProgressChanged(bool);
    void progressChanged(qreal);
};

#endif // ZMSEARCH_H
