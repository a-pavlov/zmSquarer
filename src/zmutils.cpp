#include "zmutils.h"

quint32 ZMUtils::SUB_NET_HOSTS_COUNT = 253;

QList<QHostAddress> ZMUtils::genTestHosts(const QList<QString>& addresses) {
    QList<QHostAddress> res;
    std::for_each(addresses.begin(), addresses.end(),
                   [&res](const QString& a) {
        QHostAddress myAddress = QHostAddress(a);
        QPair<QHostAddress, int> starting = QHostAddress::parseSubnet(a + "/24");
        for(quint32 i = 1; i <= 254; ++i) {
            QHostAddress current(starting.first.toIPv4Address() + i);
            if (current != myAddress) {
                res.append(current);
            }
        }
    });

    return res;
}
