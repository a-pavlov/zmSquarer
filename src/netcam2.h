#ifndef NETCAM_H
#define NETCAM_H

#include <QObject>
#include <QUrl>
#include <QThread>
#include "rsplitter.h"

class QTcpSocket;
struct http_parser;

class NetCam : public QObject {
    Q_OBJECT
public:
    explicit NetCam(const QString& url, QObject *parent = nullptr);
    ~NetCam();
    RSplitter& splitter() {
        return rsp;
    }
private:
      static int url_callback(http_parser* p, const char* c, unsigned long len);
      static int header_field_callback(http_parser* p, const char* c, unsigned long len);
      static int header_value_callback(http_parser* p, const char* c, unsigned long len);
      static int headers_complete(http_parser* p);      
      QTcpSocket* socket;
      QUrl url;
      size_t headersEndOffset;
      quint64 headerBytesRead;
      std::vector<char> headersBuffer;
      QStringList hdrKeys;
      QStringList hdrValues;
      unsigned long lastHeaderValueOffset;
      bool getCLValue;
      QString contentType;
      QString boundary;
      RSplitter rsp;
signals:

public slots:
      void start();
};

class NetCamThread : public QThread {
    Q_OBJECT
signals:
    void resultReady(const QString &s);
public slots:
    void createDT();
};

class Controller: public QObject {
    Q_OBJECT
private:
    QList<NetCam*> netcams;
    ~Controller();
public slots:
    void test();
};



#endif // NETCAM_H
