#ifndef TILEMODEL_H
#define TILEMODEL_H

#include <QAbstractListModel>
#include "zmdata.h"

class TileModel : public QAbstractListModel {
    Q_OBJECT
public:
    typedef QPair<qint32, qint32> TILE;
    typedef QPair<qint32, TILE> TILE_NUM;
private:
    QList<TILE> tiles;
    QList<ZMMonitor> availableMons;
    QString zmBaseUrl;

    bool isValidMonIndex(qint32 indx) const {
        return (indx != -1 && indx < availableMons.size());
    }
public:
    static const qint32 CAM_LINE = 4;

    enum TileRoles {
        SelectorRole   = Qt::UserRole + 1,
        PosIndexRole,
        NameRole,
        IdRole,
        StatusRole,
        ResolutionRole,
        NameRoleHR,
        IdRoleHR,
        StatusRoleHR,
        ResolutionRoleHR,
        HaveMonRole,
        HaveMonRoleHR,
        UpdateRole,
        UpdateHiResRole
    };

    static void registerQmlType();
    TileModel(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant &value, int role = Qt::EditRole) override;
    Q_INVOKABLE void setAvailableMons(const QList<ZMMonitor>& am, const QString zmBaseUrl);
    Q_PROPERTY(qint32 numeratedTilesCount READ getNumeratedTilesCount NOTIFY numeratedTilesChanged)
    Q_INVOKABLE QList<QList<TILE_NUM> > getNumeratedTiles() const;
    qint32 getNumeratedTilesCount() const;
    Q_INVOKABLE void load();
    Q_INVOKABLE void save() const;
    const QString& baseUrl() const {
        return zmBaseUrl;
    }
signals:
    void numeratedTilesChanged();
};

#endif // TILEMODEL_H
