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
    QString camVideoProducer = readFile(":/text/camvideoproducer.txt");
    QString camVideoOutput = readFile(":/text/videooutput.txt");
    QString camViewHeader = readFile(":/text/header.txt");

    int height = monLines.size();
    QFile resFile("c:/dev/test.qml");
    bool resOpened = resFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream resStream(&resFile);
    QString buffer;
    QTextStream bufferStream(&buffer);
    bufferStream << camViewHeader << "\n";
    resStream << camViewHeader << "\n";
    QString topAnchor("parent.top");

    for(const QList<ZMMonitor>& m: monLines) {
        QStringList videoProducer;
        std::transform(m.begin(), m.end(), std::back_inserter(videoProducer), [&](const ZMMonitor& mon) ->
                       QString { return camVideoProducer.arg(mon.id).arg(zmc->getMonitorUrl(mon.id.toInt()));});

        QString leftAnchor("parent.left");
        QString output("output_%1.right");

        QStringList videoOutput;
        std::transform(m.begin(), m.end(), std::back_inserter(videoOutput), [&](const ZMMonitor& mon) ->
                       QString {
            QString res = camVideoOutput
                        .arg(mon.id)
                        .arg(leftAnchor)
                        .arg(topAnchor)
                        .arg(m.size())
                        .arg(height)
                        .arg(mon.id);
            leftAnchor = QString("output_%1.right").arg(mon.id);
            return res;
        });

        topAnchor = QString("output_%1.bottom").arg(m.begin()->id);

        bufferStream << videoProducer.join("\n") << "\n\n" << videoOutput.join("\n") << "\n";
        resStream << videoProducer.join("\n") << "\n\n" << videoOutput.join("\n") << "\n";
    }

    resStream << "}\n";
    bufferStream << "}\n";

    if (resFile.isOpen()) resFile.close();

    qDebug() << "buffer size " << buffer.size();
    qDebug() << "template " << readFile(":/text/template.txt");
    qDebug() << "video provider " << readFile(":/text/camvideoproducer.txt");
    qDebug() << "video output " << readFile(":/text/videooutput.txt");

    emit success(buffer);
    return buffer;
}