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
    QList<bool> checked;
public:
    enum ServerRoles {
        IdRole   = Qt::UserRole + 1,
        NameRole,
        HostRole,
        PathRole,
        SizeRole,
        StatusRole,
        CaptureFPSRole,
        CheckedRole
    };

    static void registerQmlType();
    MonitorModel(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant &value, int role = Qt::EditRole);
    QModelIndex getIndex(const QString& id) const;

    Q_INVOKABLE QString getCheckedMonId(int) const;
    Q_INVOKABLE int getCheckedCount() const;

    Q_PROPERTY(int checkedCount READ getCheckedCount NOTIFY checkedCountChanged)

    void save() const;    // save servers list to disk
    void load();    // load servers list from disk
    Q_INVOKABLE void clean();
    Q_INVOKABLE void add(const ZMMonitor& mon);
    Q_INVOKABLE void addAll(const QList<ZMMonitor>&);
signals:
    void dataIncoming(int mc);
    void checkedCountChanged();
public slots:
    void onMonitors(const QList<ZMMonitor>&);
};

#endif //__MONITOR_MODEL__
