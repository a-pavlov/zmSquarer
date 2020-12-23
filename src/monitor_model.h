#ifndef __MONITOR_MODEL__
#define __MONITOR_MODEL__

#include <QAbstractListModel>
#include <QList>
#include <QHash>
#include <QByteArray>

#include "zmdata.h"

class MonitorModel : public QAbstractListModel {
    Q_OBJECT
private:
    QList<ZMMonitor>  monitors;
public:
    enum ServerRoles {
        IdRole   = Qt::UserRole + 1,
        NameRole,
        HostRole,
        PathRole,
        SizeRole
    };


    MonitorModel(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QModelIndex getIndex(const QString& id) const;

    void save() const;    // save servers list to disk
    void load();    // load servers list from disk
    Q_INVOKABLE void clean();
    Q_INVOKABLE void add(const ZMMonitor& mon);
    Q_INVOKABLE void addAll(const QList<ZMMonitor>&);
public slots:
    void onMonitors(const QList<ZMMonitor>&);
};

#endif //__MONITOR_MODEL__
