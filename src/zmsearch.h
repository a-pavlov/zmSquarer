#ifndef ZMSEARCH_H
#define ZMSEARCH_H

#include <QObject>
#include <QAbstractListModel>
#include <QHostAddress>
#include "zmdata.h"

class ZMSearch : public QAbstractListModel {
    Q_OBJECT
private:
    void continueSearch();
    int requestsInProgress;
    quint32 totalRequests;
    QList<QHostAddress> pendingRequests;
    QList<QPair<QString, ZMVersion>> hosts;
    void startRequest(const QHostAddress&);
    quint32 seachListSize;
public:
    static int MAX_SEARCH_REQ;
    static int SUBNET_SIZE;
    static void registerQmlType();
    ZMSearch(QObject* parent = nullptr);
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void search(const QList<QString>& interfaces);
    Q_INVOKABLE void cancel();
    int getTotalRequests() const;
    int getCompletedRequests() const;

signals:
    void found(QString);
};

#endif // ZMSEARCH_H
