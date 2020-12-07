#ifndef NETCAM_H
#define NETCAM_H

#include <QObject>
#include <QUrl>
#include "rsplitter.h"

class QTcpSocket;
struct http_parser;

class NetCam : public QObject {
    Q_OBJECT
public:
    explicit NetCam(const QString& url, QObject *parent = nullptr);
    void start();
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
};

#endif // NETCAM_H
