#include "scenebuilder.h"
#include <QtQml/qqml.h>

void SceneBuilder::registerQmlType() {
    qmlRegisterType<SceneBuilder>(
        "SceneBuilder", 0, 1, "SceneBuilder" );
}

SceneBuilder::SceneBuilder() {

}

QString SceneBuilder::buildScene(ZMClient* zmc, MonitorModel* monmod) const {
    Q_UNUSED(zmc);
    Q_UNUSED(monmod);
    return "import QtQuick 2.0; Rectangle {color: \"red\"; anchors.fill: parent; }";
}
