#include "scenebuilder.h"
#include <QtQml/qqml.h>
#include <QDebug>
#include <algorithm>

//#define TOFILE 1

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

    for(int i = 0; i < mons.size(); ++i) {
         if (mons[i].type == CamType::NEW_LINE || mons[i].type == CamType::END_SCREEN) {
            if (i == 0) {
                emit fail("New line of end of screen in the beginning of the scene");
                return QString();
            } else {
                if (mons[i-1].type == CamType::NEW_LINE || mons[i-1].type == CamType::END_SCREEN) {
                    emit fail("Adjacent elements are new line or end of screen");
                    return QString();
                }
            }
        }

        if (mons[i].type == CamType::END_SCREEN) break;
    }

    //for(const ZMMonitor& zmm: mons) {
    //    qDebug() << "zmm " << zmm.visualIndex << ":" << zmm.id << " color " << zmm.colorIndex << " cam color index " << ((zmm.type==CamType::CAM)?monmod->camColorIndex(zmm.colorIndex):1000);
    //}

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
#ifdef TOFILE
    QFile resFile("c:/dev/test.qml");
    bool resOpened = resFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream resStream(&resFile);
    resStream << camViewHeader << "\n";
#endif

    QString buffer;
    QTextStream bufferStream(&buffer);
    bufferStream << camViewHeader << "\n";
    QString topAnchor("parent.top");

    for(const QList<ZMMonitor>& m: monLines) {
        QStringList videoProducer;
        std::transform(m.begin(), m.end(), std::back_inserter(videoProducer), [&](const ZMMonitor& mon) ->
                       QString {
            return camVideoProducer
                    .arg(mon.id)
                    .arg(zmc->getMonitorUrl(mon.id.toInt()))
                    // search from the end of monitors to the beginning by the color index. if no such index - self index returns
                    .arg(zmc->getMonitorUrl((std::find_if(mons.rbegin(), mons.rend(), [&](const ZMMonitor& mon2) -> bool { return (mon.type==CamType::CAM&&mon2.type==CamType::CAM)?(monmod->camColorIndex(mon2.colorIndex) == monmod->camColorIndex(mon.colorIndex)):false;}))->id.toInt()));
        });

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
                        .arg(height);

            leftAnchor = QString("output_%1.right").arg(mon.id);
            return res;
        });

        topAnchor = QString("output_%1.bottom").arg(m.begin()->id);

        bufferStream << videoProducer.join("\n") << "\n\n" << videoOutput.join("\n") << "\n";
#ifdef TOFILE
        resStream << videoProducer.join("\n") << "\n\n" << videoOutput.join("\n") << "\n";
#endif
    }

    bufferStream << "}\n";

#ifdef TOFILE
    resStream << "}\n";
    if (resFile.isOpen()) resFile.close();
    qDebug() << "buffer size " << buffer.size();
    qDebug() << "template " << readFile(":/text/template.txt");
    qDebug() << "video provider " << readFile(":/text/camvideoproducer.txt");
    qDebug() << "video output " << readFile(":/text/videooutput.txt");
#endif

    emit success(buffer);
    return buffer;
}
