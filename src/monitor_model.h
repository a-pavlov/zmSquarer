#ifndef __MONITOR_MODEL__
#define __MONITOR_MODEL__

#include <QAbstractListModel>
#include <QList>
#include <QHash>
#include <QByteArray>

#include "zmdata.h"
#include "colormatrix.h"

class MonitorModel : public QAbstractListModel {
    Q_OBJECT
private:
    QList<ZMMonitor>  monitors;
    QList<bool> checked;
    ColorMatrix colorMatrix;
    QList<int> colorProxy;
public:
    enum ServerRoles {
        IdRole   = Qt::UserRole + 1,
        NameRole,
        HostRole,
        PathRole,
        SizeRole,
        StatusRole,
        CaptureFPSRole,
        CheckedRole,
        TypeRole,
        ColorRole,
        VisualIndexRole
    };

    static void registerQmlType();
    MonitorModel(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex& index, const QVariant &value, int role = Qt::EditRole) override;
    QModelIndex getIndex(const QString& id) const;

    Q_INVOKABLE QString getCheckedMonId(int) const;
    Q_INVOKABLE int getCheckedCount() const;

    Q_PROPERTY(int checkedCount READ getCheckedCount NOTIFY checkedCountChanged)

    Q_INVOKABLE void save() const;    // save servers list to disk
    Q_INVOKABLE void load();    // load servers list from disk
    Q_INVOKABLE void clean();
    Q_INVOKABLE void add(const ZMMonitor& mon);
    Q_INVOKABLE void addAll(const QList<ZMMonitor>&);
    Q_INVOKABLE void testAdd();
    Q_INVOKABLE void addNewLine();
    Q_INVOKABLE void addStopper();
    Q_INVOKABLE void remove(int);
    Q_INVOKABLE void changeColor(int);
    Q_INVOKABLE void setVisualIndex(int modelIndex, int visualIndex);
    Q_INVOKABLE void clear();
signals:
    void dataIncoming(int mc);
    void checkedCountChanged();
public slots:
    void onMonitors(const QList<ZMMonitor>&);
};

#endif //__MONITOR_MODEL__
