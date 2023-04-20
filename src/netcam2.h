#ifndef NETCAM_H
#define NETCAM_H

#include <QObject>
#include <QUrl>
#include <QThread>
#include "rsplitter.h"

class QTcpSocket;
class QTimer;
struct http_parser;

class NetCam : public QObject {
    Q_OBJECT
public:
    explicit NetCam(int, QObject *parent = nullptr);
    ~NetCam();
    RSplitter& splitter() {
        return rsp;
    }

    QSharedPointer<RBuffer> getImageBuffer();
private:
      static int url_callback(http_parser* p, const char* c, unsigned long len);
      static int header_field_callback(http_parser* p, const char* c, unsigned long len);
      static int header_value_callback(http_parser* p, const char* c, unsigned long len);
      static int headers_complete(http_parser* p);
      QTcpSocket* socket;
      int monitorId;
      size_t headersEndOffset;
      size_t headerBytesRead;
      std::vector<char> headersBuffer;
      QStringList hdrKeys;
      QStringList hdrValues;
      unsigned long lastHeaderValueOffset;
      bool getCLValue;
      QString contentType;
      QString boundary;
      RSplitter rsp;
      int failCount;
      QTimer* watchdog;
      QTime tmImgReq;
      QTime nextConnTime;
      void connect();
signals:
      void error();
      void success();
      void disconnected();
public slots:
      void start();
private slots:
      void checkState();
};

#endif // NETCAM_H
