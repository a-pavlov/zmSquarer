#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>

#include"DesktopVideoProducer.h"
#include "http_parser.h"
#include "netcam2.h"
#include "zmsqapplication.h"
#include "camcontroller.h"
#include "camvideoproducer.h"

int my_url_callback_(http_parser* p, const char* c, unsigned long len) {
    qDebug() << "on url " << QString::fromLatin1(c, len);
    return 0;
}

int my_header_field_callback_(http_parser* p, const char* c, unsigned long len) {
    qDebug() << "on header " << QString::fromLatin1(c, len);
    return 0;
}

int my_header_value_callback_(http_parser* p, const char* c, unsigned long len) {
    qDebug() << "on header value " << QString::fromLatin1(c, len);
    return 0;
}

int my_headers_complete_(http_parser* p) {
    qDebug() << "on headers complete " << p->nread;
    return 0;
}


int main(int argc, char *argv[]) {
    DesktopVideoProducer::registerQmlType();
    CamVideoProducer::registerQmlType();
    QThread* nt = new QThread();
    QObject::connect(nt, &QThread::started, [](){
       qDebug() << "network thread started " << QThread::currentThread()->currentThreadId();
    });

    nt->start();
    CamController* cc = new CamController();
    cc->moveToThread(nt);

    ZMSQApplication app(argc, argv, nt, cc);

    qDebug() << "app thread id " << app.thread()->currentThreadId();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));


/*
    bool headersCompleted = false;
    QUrl url("http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=1&scale=100&maxfps=30&buffer=1000&user=admin&pass=root");

    std::vector<char> data(1024);
    int bytesReceived = 0;
    QTcpSocket* socket = new QTcpSocket;

    QObject::connect(socket, &QTcpSocket::connected,[=](){
       qDebug() << "connected";
       qDebug() << "connect string ";
       QByteArray array;
       array.append("GET ")
               .append(url.path().toStdString().c_str())
               .append("?")
               .append(url.query().toStdString().c_str())
               .append(" HTTP/1.0\r\nHost: 192.168.100.12\r\nUser-Agent: Motion-netcam/4.3.1+git20201114-dc670f5\r\nConnection: close\r\n\r\n");

       //QString request = "GET " + url.path() + "?" + url.query() + " HTTP/1.0\nHost: 192.168.100.12\nUser-Agent: Motion-netcam/4.3.1+git20201114-dc670f5\nConnection: close\n\n";
       qDebug() << "request " << array.constData();
       socket->write(array.constData(), array.size());
    });

    QObject::connect(socket, &QTcpSocket::readyRead, [&]() {
        int bytes = socket->bytesAvailable();

        qDebug() << "bytes available " << bytes;
        if (headersCompleted) {
            std::vector<char> buff(bytes);

            // just skip it now
            int r = socket->read(&buff[0], bytes);
            qDebug() << "read next " << r;
        } else {
            int b = socket->read(&data[0] + bytesReceived, qMin((int)data.size() - bytesReceived, bytes));
            qDebug() << "read bytes " << b;
            bytesReceived += b;

            if (bytesReceived == data.size()) {
                http_parser parser;
                http_parser_init(&parser, HTTP_RESPONSE);
                http_parser_settings settings;
                http_parser_settings_init(&settings);
                settings.on_url = my_url_callback_;
                settings.on_header_field = my_header_field_callback_;
                settings.on_header_value = my_header_value_callback_;
                settings.on_headers_complete = my_headers_complete_;

                size_t nparsed = http_parser_execute(&parser, &settings, &data[0], data.size());
                qDebug() << "parsed " << nparsed << " input " << data.size();
                headersCompleted = true;
            }
        }

    });

    QObject::connect(socket, &QTcpSocket::disconnected, [=] () {
        qDebug()<< "DISCONNECTED ";
        socket->deleteLater();
    });

    QObject::connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
    (&QAbstractSocket::error), [=](QAbstractSocket::SocketError) {
        qDebug()<< "ERROR " << socket->errorString();
        socket->deleteLater();
    });*/

    //socket->setReadBufferSize(400);
    //socket->connectToHost(url.host(), url.port()!=-1?url.port():80);
    //qDebug() << "connect to " << url.host() << " port " << (url.port()!=-1?url.port():80);

    //NetCam* netcam = new NetCam("http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=1&scale=100&maxfps=30&buffer=1000&user=admin&pass=root");
    //NetCam* netcam2 = new NetCam("http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=1&scale=100&maxfps=30&buffer=1000&user=admin&pass=root");
    //netcam->moveToThread(nct);
    //QObject::connect(nct, &QThread::started, netcam, &NetCam::start);
    //QObject::connect(nct, &QThread::started, netcam2, &NetCam::start);
    //QObject::connect(nct, &QThread::finished, netcam, &QObject::deleteLater);
    //QObject::connect(nct, &QThread::finished, netcam2, &QObject::deleteLater);
    //netcam->moveToThread(nct);
    //netcam2->moveToThread(nct);    

    //QObject::connect(nct, &QThread::started, [](){
    //    qDebug() << " started " << QThread::currentThread()->currentThreadId();
    //});

    //QTimer timer;
    //timer.moveToThread(nct);
    //QObject::connect(&timer, &QTimer::timeout, [=](){
    //    QMetaObject::invokeMethod(nct, "createDT", Qt::QueuedConnection);
    //});

    //QObject::connect(&timer, SIGNAL(timeout()), ctr, SLOT(test()), Qt::QueuedConnection);

    //QObject::connect(&timer, SIGNAL(timeout()), nct, SLOT(createDT()), Qt::QueuedConnection);
    //timer.start(20000);

   // QMetaObject::invokeMethod(nct, "createDT", Qt::QueuedConnection);
    return app.exec();
}
