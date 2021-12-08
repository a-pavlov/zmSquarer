#ifndef TILEMODEL_H
#define TILEMODEL_H

#include <QAbstractListModel>
#include "zmdata.h"

class TileModel : public QAbstractListModel {
    Q_OBJECT
    QList<qint32> tiles;
    QList<ZMMonitor> availableMons;

    bool isValidMonIndex(qint32 indx) const {
        return (indx != -1 && indx < availableMons.size());
    }
public:
    static const quint32 CAM_LINE = 4;

    enum TileRoles {
        SelectorRole   = Qt::UserRole + 1,
        PosIndexRole,
        NameRole,
        HaveMonRole,
        UpdateRole
    };

    static void registerQmlType();
    TileModel(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant &value, int role = Qt::EditRole) override;
    Q_INVOKABLE void setAvailableMons(const QList<ZMMonitor>& am);
};

#endif // TILEMODEL_H
