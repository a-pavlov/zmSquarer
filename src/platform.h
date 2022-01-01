#ifndef PLATFORM_H
#define PLATFORM_H

#include <QObject>

class Platform : public QObject {
    Q_OBJECT
public:
    explicit Platform(QObject *parent = nullptr);
    static void registerQmlType();
    Q_PROPERTY(QString name READ getName NOTIFY platformChanged)
    QString getName() const;
signals:
    void platformChanged();
};

#endif // PLATFORM_H
