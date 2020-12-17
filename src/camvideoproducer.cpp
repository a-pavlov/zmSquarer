#include "camvideoproducer.h"

#include <QtQml/qqml.h>

#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QDebug>
#include <QDir>
#include <QSharedPointer>


#include "zmsqapplication.h"
#include "netcam2.h"


#include <stdio.h>


void CamVideoProducer::registerQmlType() {
    qmlRegisterType<CamVideoProducer>(
        "CamVideoProducer", 0, 1, "CamVideoProducer" );
}

CamVideoProducer::CamVideoProducer(QObject *parent )
    : QObject( parent ), _surface( 0 ), netCam(nullptr) {
    startTimer( 1000 / 10 ); //15 fps
    counter = 0;
#ifdef WITH_TURBOJPEG
    _jpegDecompressor = tjInitDecompress();
#endif
}

CamVideoProducer::~CamVideoProducer() {
    closeSurface();
#ifdef WITH_TURBOJPEG
    tjDestroy(_jpegDecompressor);
#endif
}

QAbstractVideoSurface* CamVideoProducer::videoSurface() const {
    return _surface;
}

QString CamVideoProducer::url() const {
    return QString(urlCurrent.c_str());
}

QString CamVideoProducer::setUrl(const QString& u) {
    ZMSQApplication* app = static_cast<ZMSQApplication*>(QApplication::instance());
    qDebug() << "video producer set url " << u << " thread ";
    netCam = app->getCamController().startCam(u);
    urlCurrent = u.toStdString();
    return u;
}

void CamVideoProducer::setVideoSurface( QAbstractVideoSurface* s )
{
    closeSurface();
    _surface = s;
}

void CamVideoProducer::closeSurface()
{
    if( _surface && _surface->isActive() )
        _surface->stop();
}

void CamVideoProducer::timerEvent( QTimerEvent* )
{
    if( !_surface )
        return;

    QScreen* screen = QGuiApplication::primaryScreen();
    QDesktopWidget* desktop = QApplication::desktop();

    if( !screen || !desktop )
        return;

    if (netCam == nullptr) {
        qDebug() << "netcam is null";
        return;
    }


    QSharedPointer<RBuffer> ptr = netCam->splitter().getOutputBuffer();

    // check we have the new buffer
    if (ptr.isNull()) {
        return;
    }

    unsigned char* data = (unsigned char*)ptr->getPtr();

    //FILE* f = fopen(QString("c:\\dev\\img\\file_%1.jpg").arg(++counter).toStdString().c_str(), "wb+");
    //if (f != nullptr) {
    //    int bytes  = fwrite(ptr->getPtr() + ptr->getSoiPos(), ptr->getContentLength() - ptr->getSoiPos(), 1, f);
    //    fclose(f);
    //    qDebug() << "write " << bytes;
    //}
#ifdef WITH_TURBOJPEG
    int width, height, jpegSubsamp, jpegColorspace;
    TJPF pf = TJPF_BGRX;

    if (tjDecompressHeader3(_jpegDecompressor, data + ptr->getSoiPos()
                            , static_cast<unsigned long>(ptr->getContentLength() - ptr->getSoiPos())
                            , &width
                            , &height
                            , &jpegSubsamp
                            , &jpegColorspace) == 0) {
        buffer.resize(static_cast<size_t>(width * height * tjPixelSize[pf]));
        if (tjDecompress2(_jpegDecompressor
                          , data + ptr->getSoiPos()
                          , static_cast<unsigned long>(ptr->getContentLength() - ptr->getSoiPos())
                          , &buffer[0]
                          , width
                          , 0/*pitch*/
                          , height
                          , pf
                          , TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == 0) {
           QImage screenImage(&buffer[0], width, height, tjPixelSize[pf]*width, QImage::Format_RGB32);
           QVideoFrame::PixelFormat pixelFormat = QVideoFrame::pixelFormatFromImageFormat( screenImage.format() );

           if( screenImage.size() != _format.frameSize() ||
               pixelFormat != _format.pixelFormat() ) {
               closeSurface();
               _format =
                   QVideoSurfaceFormat( screenImage.size(),
                                        pixelFormat );
               _surface->start( _format );
           }

           _surface->present( QVideoFrame( screenImage ) );
        } else {
            qDebug() << "unable to decompress image";
        }
    } else {
        int err = tjGetErrorCode(_jpegDecompressor);
        qDebug() << "unable to decompress header " << err;
    }
#else


    qDebug() << "incoming size " << ptr->getContentLength() - ptr->getSoiPos();
    QImage screenImage; //("/home/inkpot/dev/" + frames.at(currentFrame++ % frames.size()));

    bool res = screenImage.loadFromData(data + ptr->getSoiPos(), ptr->getContentLength() - ptr->getSoiPos(), "JPG");
    if (res) {
        QVideoFrame::PixelFormat pixelFormat = QVideoFrame::pixelFormatFromImageFormat( screenImage.format() );

        //если формат кадра по какой-то причине поменялся (или это первый кадр)-
        //выполним повторную (первичную) инициализацию surface
        if( screenImage.size() != _format.frameSize() ||
            pixelFormat != _format.pixelFormat() )
        {
            closeSurface();
            _format =
                QVideoSurfaceFormat( screenImage.size(),
                                     pixelFormat );
            _surface->start( _format );
        }

        //передадим полученный кадр на отрисовку
        _surface->present( QVideoFrame( screenImage ) );
    }
#endif
}
