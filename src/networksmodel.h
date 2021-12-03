#ifndef NETWORKSMODEL_H
#define NETWORKSMODEL_H

#include <QObject>
#include <QAbstractListModel>

class NetworksModel : public QAbstractListModel {
    Q_OBJECT
private:
    QList<QString> addresses;
public:
    enum ServerRoles {
        AddressRole   = Qt::UserRole + 1
    };

    static void registerQmlType();
    NetworksModel(QObject* parent = nullptr);
    Q_INVOKABLE void refresh();
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

};

#endif // NETWORKSMODEL_H
