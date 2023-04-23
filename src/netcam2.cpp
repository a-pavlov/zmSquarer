#include "netcam2.h"
#include "http_parser.h"
#include "zmclient.h"
#include <QTcpSocket>
#include <QtGlobal>
#include <QTimer>
#include <QSslSocket>

#define RETRY_COUNT 3
#define IMG_REQ_TIMEOUT 5
#define WATCH_DOG_INTERVAL_SEC 5

int NetCam::url_callback(http_parser* p, const char* c, unsigned long len) {
    Q_UNUSED(c);
    Q_UNUSED(len);
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
    Q_UNUSED(p)
    return 0;
}

NetCam::NetCam(int mid, QObject *parent) : QObject(parent)
    , socket(nullptr)
    , monitorId(mid)
    , headersEndOffset(0)
    , headerBytesRead(0)
    , headersBuffer(1024)
    , lastHeaderValueOffset(0)
    , getCLValue(false)
    , failCount(0) {
    watchdog = new QTimer(this);
    QObject::connect(watchdog, SIGNAL(timeout()), this, SLOT(checkState()));
    watchdog->start(2000);
}

NetCam::~NetCam() {    
    if(socket != nullptr) {        
        socket->deleteLater();
    }    
}

void NetCam::start() {
    connect();
}

void NetCam::connect() {
    headerBytesRead = 0;
    rsp.reset();
    nextConnTime = QTime();

    QUrl url = ZMClient::getMonitorUrl(monitorId);

    if (socket == nullptr) {
        socket = (url.scheme() == "https")?new QSslSocket:new QTcpSocket;

        if (url.scheme() == "https") {
            QSslSocket* sslSocket = dynamic_cast<QSslSocket*>(socket);

            QObject::connect(sslSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
                [=](const QList<QSslError> &errors){
                    QList<QSslError> canBeIgnoredErrors;
                    canBeIgnoredErrors << QSslError::HostNameMismatch << QSslError::SelfSignedCertificate;
                    for (const QSslError& sslError: errors) {
                        if (!canBeIgnoredErrors.contains(sslError.error())) return;
                    }

                    sslSocket->ignoreSslErrors(errors);
                }
            );
        }

        QObject::connect(socket, &QTcpSocket::connected, this, [=](){
           QByteArray array;
           array.append("GET ")
                   .append(url.path().toStdString().c_str())
                   .append("?")
                   .append(url.query().toStdString().c_str())
                   .append(" HTTP/1.0\r\nHost: " + url.host().toLocal8Bit() + "\r\nUser-Agent: zmSquarer-netcam/0.1\r\nConnection: close\r\n\r\n");
           failCount = 0;
           socket->write(array.constData(), array.size());
           emit success();
        });

        QObject::connect(socket, &QTcpSocket::readyRead, this, [&]() {
            qint64 bytesAvailable = socket->bytesAvailable();
            //qDebug() << "read ready, fail count reset";
            //qDebug() << "bytes available " << bytesAvailable;

            while(bytesAvailable > 0) {
                if (headerBytesRead < headersBuffer.size()) {
                    size_t bytesToRead = qMin(static_cast<size_t>(bytesAvailable), headersBuffer.size() - headerBytesRead);
                    qint64 readBytes = socket->read(&headersBuffer[0] + headerBytesRead, static_cast<qint64>(bytesToRead));

                    if (readBytes == -1) {
                        // error
                    }

                    headerBytesRead += static_cast<size_t>(readBytes);
                    bytesAvailable -= readBytes;

                    if (headerBytesRead == headersBuffer.size()) {
                        // start headers processing
                        http_parser parser;
                        parser.data = this;
                        http_parser_init(&parser, HTTP_RESPONSE); /* initialise parser */
                        http_parser_settings settings;
                        http_parser_settings_init(&settings);
                        settings.on_url = (http_data_cb)&NetCam::url_callback;
                        settings.on_header_field = (http_data_cb)&NetCam::header_field_callback;
                        settings.on_header_value = (http_data_cb)&NetCam::header_value_callback;
                        settings.on_headers_complete = &NetCam::headers_complete;
                        size_t nparsed = http_parser_execute(&parser, &settings, &headersBuffer[0], headersBuffer.size());
                        Q_UNUSED(nparsed);
                        //qDebug() << "parsed " << nparsed << " input " << headersBuffer.size() << " last header value pos " << lastHeaderValueOffset;
                        //qDebug() << "remain " << headersBuffer.size() - lastHeaderValueOffset;
                        //qDebug() << QString::fromLocal8Bit(&headersBuffer[lastHeaderValueOffset], qMin<size_t>(20ul, headersBuffer.size() - lastHeaderValueOffset));
                        Q_ASSERT(headersEndOffset < headersBuffer.size());
                        // calculate the rest of data in the buffer and write it to target
                        Q_ASSERT(hdrKeys.size() == hdrValues.size());

                        //for(int i = 0; i < hdrKeys.size(); ++i) {
                        //    qDebug() << hdrKeys[i] << "=" << hdrValues[i];
                        //}

                        //qDebug() << "boundary " << boundary;
                        //qDebug() << "content-type " << contentType;
                        this->rsp.setPattern(boundary);
                        // read the remain data in the headers buffer
                        rsp.read(&headersBuffer[lastHeaderValueOffset], headersBuffer.size() - lastHeaderValueOffset);
                        break;
                    }
                } else {
                    QSharedPointer<RBuffer> buf = this->rsp.getCurrentRBuffer();
                    Q_ASSERT(!buf.isNull());
                    char* ptr = buf->checkBufferSize(static_cast<size_t>(bytesAvailable));
                    socket->read(ptr, bytesAvailable);
                    buf->expandUsed(static_cast<size_t>(bytesAvailable));
                    rsp.processCB(buf);
                    bytesAvailable = 0;
                }
            }
        });

        QObject::connect(socket, &QTcpSocket::disconnected, this, [=] () {
            emit disconnected();
            socket->deleteLater();
            socket = nullptr;
        });

        QObject::connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
        (&QAbstractSocket::error), this, [&](QAbstractSocket::SocketError socketError) {            
            switch (socketError) {
                case QAbstractSocket::RemoteHostClosedError:
                    qDebug() << QTime::currentTime() << " The remote host closed connection for monitor: " << monitorId;
                    break;
                case QAbstractSocket::HostNotFoundError:
                    qDebug() << QTime::currentTime() << " The host was not found. Please check the host name and port settings. monitor: " << monitorId;
                    break;
                case QAbstractSocket::ConnectionRefusedError:
                    qDebug() << QTime::currentTime() << " The connection was refused by the peer. monitor: " << monitorId;
                    break;
                default:
                    qDebug() << socket->errorString();
            }

            emit error();
            ++failCount;
            auto secondsToReconn = failCount > 5?5*4:failCount*4;
            nextConnTime = QTime::currentTime().addSecs(secondsToReconn);
        });
    }


    if (url.scheme() == "https") {
        (static_cast<QSslSocket*>(socket))->connectToHostEncrypted(url.host(), url.port(443));
    } else {
        socket->connectToHost(url.host(), url.port(80));
    }
}

QSharedPointer<RBuffer> NetCam::getImageBuffer() {
    tmImgReq = QTime::currentTime();
    return splitter().getOutputBuffer();
}

void NetCam::checkState() {
    if (!nextConnTime.isNull() && nextConnTime < QTime::currentTime()) {
        nextConnTime = QTime();
        start();
    }

    if (socket != nullptr) {
        if (!tmImgReq.isNull()) {
            if (tmImgReq.secsTo(QTime::currentTime()) > IMG_REQ_TIMEOUT) {
                qDebug() << "idle timeout reached, stop connection, monitor: " << monitorId;
                socket->close();
                rsp.reset();
                tmImgReq = QTime();
            }
        }
    }
}
