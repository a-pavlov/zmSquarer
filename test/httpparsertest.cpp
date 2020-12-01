#include "httpparsertest.h"
#include "http_parser.h"
#include <QDebug>

HttpParserTest::HttpParserTest(QObject *parent) : QObject(parent)
{

}

const char buf[] = "HTTP/1.1 301 Moved Permanently\r\n"
                   "Location: http://www.google.com/\r\n"
                   "Content-Type: text/html; charset=UTF-8\r\n"
                   "Date: Sun, 26 Apr 2009 11:11:49 GMT\r\n"
                   "Expires: Tue, 26 May 2009 11:11:49 GMT\r\n"
                   "Cache-Control: public, max-age=2592000\r\n"
                   "Server: gws\r\n"
                   "Content-Length: 219\r\n"
                   "\r\n"
                   "<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"
                   "<TITLE>301 Moved</TITLE></HEAD><BODY>\n"
                   "<H1>301 Moved</H1>\n"
                   "The document has moved\n"
                   "<A HREF=\"http://www.google.com/\">here</A>.\r\n"
                   "</BODY></HTML>\r\n";

int my_url_callback(http_parser* p, const char* c, unsigned long len) {
    qDebug() << "on url " << QString::fromLatin1(c, len);
    return 0;
}

int my_header_field_callback(http_parser* p, const char* c, unsigned long len) {
    qDebug() << "on header " << QString::fromLatin1(c, len) << " pos: " << (c - buf);
    return 0;
}

int my_header_value_callback(http_parser* p, const char* c, unsigned long len) {
    qDebug() << "on header value " << QString::fromLatin1(c, len) << " pos: " << (c - buf);
    return 0;
}

int my_headers_complete(http_parser* p) {
    qDebug() << "on headers complete " << p->nread;
    return 0;
}



void HttpParserTest::test() {
    http_parser parser;
    http_parser_init(&parser, HTTP_RESPONSE); /* initialise parser */
    http_parser_settings settings;
    http_parser_settings_init(&settings);
    settings.on_url = my_url_callback;
    settings.on_header_field = my_header_field_callback;
    settings.on_header_value = my_header_value_callback;
    settings.on_headers_complete = my_headers_complete;

    size_t nparsed = http_parser_execute(&parser, &settings, buf, strlen(buf));
    qDebug() << "parsed " << nparsed << " input " << strlen(buf);
}
