#include "netcam2.h"
#include "http_parser.h"
#include <QTcpSocket>
#include <QtGlobal>

int NetCam::url_callback(http_parser* p, const char* c, unsigned long len) {
    NetCam* pnc = static_cast<NetCam*>(p->data);
    Q_ASSERT(pnc != nullptr);
    return 0;
}

int NetCam::header_field_callback(http_parser* p, const char* c, unsigned long len) {
    NetCam* pnc = static_cast<NetCam*>(p->data);
    Q_ASSERT(pnc != nullptr);
    pnc->hdrKeys.append(QString::fromLocal8Bit(c, static_cast<int>(len)));
    if (len == strlen("Content-Type") && strncmp(c, "Content-Type", len) == 0) {
        pnc->getCLValue = true;
    }
    return 0;
}

int NetCam::header_value_callback(http_parser* p, const char* c, unsigned long len) {
    NetCam* pnc = static_cast<NetCam*>(p->data);
    Q_ASSERT(pnc != nullptr);
    QString value = QString::fromLocal8Bit(c, static_cast<int>(len));
    pnc->hdrValues.append(value);
    pnc->lastHeaderValueOffset = static_cast<unsigned long>(c - &(pnc->headersBuffer[0])) + len;
    if (pnc->getCLValue) {
        QStringList arguments = value.split(";");
        if (arguments.size() == 2) {
            pnc->contentType = arguments[0];
            if (arguments[1].startsWith("boundary=")) {
                pnc->boundary = arguments[1].right(arguments[1].length() - 9);
            }
        }
    }
    return 0;
}

int NetCam::headers_complete(http_parser* p) {
    qDebug() << "headers completed";
    return 0;
}

NetCam::NetCam(const QString& u, QObject *parent) : QObject(parent)
    , url(u)
    , headersEndOffset(0)
    , headerBytesRead(0)
    , headersBuffer(1024)
    , lastHeaderValueOffset(0)
    , getCLValue(false)
    , rsp("") {

    socket = new QTcpSocket;

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
        qint64 bytesAvailable = socket->bytesAvailable();
        //qDebug() << "bytes available " << bytesAvailable;

        while(bytesAvailable > 0) {
            if (headerBytesRead < headersBuffer.size()) {
                qint64 bytesToRead = qMin(static_cast<quint64>(bytesAvailable), headersBuffer.size() - headerBytesRead);
                qDebug() << "bytes to read " << bytesToRead;
                // read to headers
                qint64 readBytes = socket->read(&headersBuffer[0] + headerBytesRead, bytesToRead);
                qDebug() << "read bytes " << readBytes;
                if (readBytes == -1) {
                    // error
                }

                headerBytesRead += readBytes;
                bytesAvailable -= readBytes;

                if (headerBytesRead == headersBuffer.size()) {
                    // start headers processing
                    http_parser parser;
                    parser.data = this;
                    http_parser_init(&parser, HTTP_RESPONSE); /* initialise parser */
                    http_parser_settings settings;
                    http_parser_settings_init(&settings);
                    settings.on_url = &NetCam::url_callback;
                    settings.on_header_field = &NetCam::header_field_callback;
                    settings.on_header_value = &NetCam::header_value_callback;
                    settings.on_headers_complete = &NetCam::headers_complete;
                    size_t nparsed = http_parser_execute(&parser, &settings, &headersBuffer[0], headersBuffer.size());
                    qDebug() << "parsed " << nparsed << " input " << headersBuffer.size() << " last header value pos " << lastHeaderValueOffset;
                    qDebug() << "remain " << headersBuffer.size() - lastHeaderValueOffset;
                    qDebug() << QString::fromLocal8Bit(&headersBuffer[lastHeaderValueOffset], qMin(20ul, headersBuffer.size() - lastHeaderValueOffset));
                    Q_ASSERT(headersEndOffset < headersBuffer.size());
                    // calculate the rest of data in the buffer and write it to target
                    Q_ASSERT(hdrKeys.size() == hdrValues.size());

                    for(int i = 0; i < hdrKeys.size(); ++i) {
                        qDebug() << hdrKeys[i] << "=" << hdrValues[i];
                    }

                    qDebug() << "boundary " << boundary;
                    qDebug() << "content-type " << contentType;
                    this->rsp.setPattern(boundary);
                    // read the remain data in the headers buffer
                    rsp.read(&headersBuffer[lastHeaderValueOffset], headersBuffer.size() - lastHeaderValueOffset);
                    break;
                }
            } else {
                QSharedPointer<RBuffer> buf = this->rsp.getCurrentRBuffer();
                Q_ASSERT(!buf.isNull());
                char* ptr = buf->checkBufferSize(bytesAvailable);
                socket->read(ptr, bytesAvailable);
                buf->expandUsed(bytesAvailable);
                rsp.processCB(buf);
                bytesAvailable = 0;
            }
        }
    });

    QObject::connect(socket, &QTcpSocket::disconnected, [=] () {
        qDebug()<< "DISCONNECTED ";
        //socket->deleteLater();
    });

    QObject::connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
    (&QAbstractSocket::error), [=](QAbstractSocket::SocketError) {
        qDebug()<< "ERROR " << socket->errorString();
        //socket->deleteLater();
    });
}

void NetCam::start() {
    socket->connectToHost(url.host(), 80);
}
