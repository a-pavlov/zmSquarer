#ifndef ZMUTILS_H
#define ZMUTILS_H

#include <QHostAddress>

class ZMUtils
{
public:
    static quint32 SUB_NET_HOSTS_COUNT;
    static QList<QHostAddress> genTestHosts(const QList<QString>& addresses);
};

#endif // ZMUTILS_H
