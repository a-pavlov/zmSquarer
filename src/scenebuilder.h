#ifndef SCENEBUILDER_H
#define SCENEBUILDER_H

#include <QObject>
#include "monitor_model.h"
#include "zmclient.h"

class SceneBuilder: public QObject {
    Q_OBJECT
public:
    static void registerQmlType();
    static QString readFile(const QString&);
    SceneBuilder();
    Q_INVOKABLE QString buildScene(ZMClient* zmc, MonitorModel* monmod) const;
signals:
    void success(const QString& code) const;
    void fail(const QString& code) const;

};

#endif // SCENEBUILDER_H
