#ifndef NETWORKSMODEL_H
#define NETWORKSMODEL_H

#include <QObject>
#include <QAbstractListModel>

class NetworksModel : public QAbstractListModel {
    Q_OBJECT
private:
    QList<QPair<bool, QString> > addresses;
public:
    enum ServerRoles {
        AddressRole   = Qt::UserRole + 1,
        SelectedMode
    };

    static void registerQmlType();
    NetworksModel(QObject* parent = nullptr);
    Q_INVOKABLE void refresh();
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant &value, int role = Qt::EditRole) override;
    Q_PROPERTY(int selectedCount READ getSelectedCount NOTIFY selectedChanged)
    Q_INVOKABLE int getSelectedCount() const;
    Q_INVOKABLE QList<QString> getSelected() const;
signals:
    void selectedChanged(int count);
};

#endif // NETWORKSMODEL_H
