#include "scenebuilder.h"
#include <QtQml/qqml.h>
#include <QDebug>
#include <algorithm>


void SceneBuilder::registerQmlType() {
    qmlRegisterType<SceneBuilder>(
        "SceneBuilder", 0, 1, "SceneBuilder" );
}

QString SceneBuilder::readFile(const QString & filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) return QString();
    QTextStream in(&f);
    QString res = in.readAll();
    f.close();
    return res;
}

SceneBuilder::SceneBuilder() {

}

QString SceneBuilder::buildScene(ZMClient* zmc, MonitorModel* monmod) const {
    Q_UNUSED(zmc);
    Q_UNUSED(monmod);
    QList<ZMMonitor> mons;
    mons.append(monmod->getMonitors());
    std::sort(mons.begin(), mons.end(), [](const ZMMonitor& a, const ZMMonitor& b) -> bool { return a.visualIndex < b.visualIndex; });
    QList<QList<ZMMonitor>> monLines;
    QList<ZMMonitor> line;

    for(auto itr = mons.begin(); itr != mons.end(); ++itr) {
        if (itr->type == CamType::CAM) {
            line.append(*itr);
        } else if (itr->type == CamType::NEW_LINE) {
            if (line.isEmpty()) {
                // raise error
                emit fail("xxx");
            }

            monLines.append(line);
            line.clear();
        } else if (itr->type == CamType::END_SCREEN) {
            if (line.isEmpty()) {
                // raise error
                emit fail("xxx");
            }
            monLines.append(line);
            line.clear();
            break;
        }
    }

    Q_ASSERT(line.isEmpty());
    int height = monLines.size();
    for(const QList<ZMMonitor>& m: monLines) {
        QStringList out;
        std::transform(m.begin(), m.end(), std::back_inserter(out), [](const ZMMonitor& mon) -> QString { return mon.id;});
        qDebug() << out.join(",");
    }

    qDebug() << "template " << readFile(":/text/template.txt");
    qDebug() << "video provider " << readFile(":/text/camvideoproducer.txt");
    qDebug() << "video output " << readFile(":/text/videooutput.txt");

    emit success("import QtQuick 2.0; Rectangle {color: \"red\"; anchors.fill: parent; }");

    return "import QtQuick 2.0; Rectangle {color: \"red\"; anchors.fill: parent; }";
}
