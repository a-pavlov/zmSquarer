#ifndef SCENEBUILDER_H
#define SCENEBUILDER_H

#include <QObject>
#include "monitor_model.h"
#include "zmclient.h"

class SceneBuilder: public QObject {
    Q_OBJECT
public:
    static void registerQmlType();
    SceneBuilder();
    Q_INVOKABLE QString buildScene(ZMClient* zmc, MonitorModel* monmod) const;
};

#endif // SCENEBUILDER_H
