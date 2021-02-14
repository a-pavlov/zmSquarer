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
    : QObject( parent )
    , _surface( nullptr )
    , netCam(nullptr)
    , errorOnCam(false)
    , camDisconnected(true) {
    timerId = startTimer( 1000 / 10 ); //15 fps
    counter = 0;
#ifdef WITH_TURBOJPEG
    _jpegDecompressor = tjInitDecompress();
#endif
}

CamVideoProducer::~CamVideoProducer() {
    qDebug() << "cam video producer close";
    if (timerId != -1) {
        killTimer(timerId);
    }
    closeSurface();

    if (netCam) {
        netCam->deleteLater();
    }
#ifdef WITH_TURBOJPEG
    tjDestroy(_jpegDecompressor);
#endif
}

QAbstractVideoSurface* CamVideoProducer::videoSurface() const {
    return _surface;
}

QString CamVideoProducer::url() const {
    return urlCurrent;
}

QString CamVideoProducer::setUrl(const QString& u) {
    if (u.isEmpty() || u.isNull()) return u;
    ZMSQApplication* app = static_cast<ZMSQApplication*>(QApplication::instance());
    qDebug() << "video producer set url " << u << " thread ";
    netCam = app->getCamController().createCam(u);


    /*
     * disabled. need to fix cam restarting when can not connect
     * QObject::connect(netCam, &NetCam::error, [&] () {
        errorOnCam = true;
    });


    QObject::connect(netCam, &NetCam::success, [&] () {
        errorOnCam = false;
    });

    QObject::connect(netCam, &NetCam::disconnected, [&] () {
        camDisconnected = true;
    });*/

    urlCurrent = u;
    return u;
}

void CamVideoProducer::setVideoSurface( QAbstractVideoSurface* s )
{
    closeSurface();
    _surface = s;
}

void CamVideoProducer::closeSurface()
{
    if( _surface && _surface->isActive() ) {
        qDebug() << "stop surface";
        _surface->stop();
    }
}

void CamVideoProducer::drawNoSignal() {
    QImage screenImage(":/images/no_signal.jpg");
    QVideoFrame::PixelFormat pixelFormat = QVideoFrame::pixelFormatFromImageFormat( screenImage.format() );
    if( screenImage.size() != _format.frameSize() ||
        pixelFormat != _format.pixelFormat() ) {
        closeSurface();
        _format = QVideoSurfaceFormat( screenImage.size(), pixelFormat );
        _surface->start( _format );
    }

    _surface->present( QVideoFrame( screenImage ) );
}

void CamVideoProducer::updateTimer(bool start) {
    if (start) {
        if (timerId == -1) {
            timerId = startTimer( 1000 / 10 );
            qDebug() << "start timer " << timerId;
        }
    } else {
        if (timerId != -1) {
            qDebug() << "stop timer " << timerId;
            killTimer(timerId);
            timerId = -1;
        }
    }
}

void CamVideoProducer::timerEvent( QTimerEvent* ) {
    if( !_surface )
        return;

    //QScreen* screen = QGuiApplication::primaryScreen();
    //QDesktopWidget* desktop = QApplication::desktop();

    //if( !screen || !desktop )
    //    return;

    if (netCam == nullptr) {
        drawNoSignal();
        return;
    }

    QSharedPointer<RBuffer> ptr = netCam->getImageBuffer();

    // check we have the new buffer
    if (ptr.isNull()) {
        if (errorOnCam || (counter == 0)) drawNoSignal();

        // start netcam if it is still not started
        if (camDisconnected) {
            ZMSQApplication* app = static_cast<ZMSQApplication*>(QApplication::instance());
            app->getCamController().startCam(netCam);
            camDisconnected = false;
        }

        return;
    }

    Q_ASSERT(ptr->getContentLength() > ptr->getSoiPos());
    const unsigned char* data = reinterpret_cast<const unsigned char*>(ptr->getPtr() + ptr->getSoiPos());
    const unsigned long length = static_cast<unsigned long>(ptr->getContentLength() - ptr->getSoiPos());

    //FILE* f = fopen(QString("c:\\dev\\img\\file_%1.jpg").arg(++counter).toStdString().c_str(), "wb+");
    //if (f != nullptr) {
    //    int bytes  = fwrite(ptr->getPtr() + ptr->getSoiPos(), ptr->getContentLength() - ptr->getSoiPos(), 1, f);
    //    fclose(f);
    //    qDebug() << "write " << bytes;
    //}
#ifdef WITH_TURBOJPEG
    int width, height, jpegSubsamp, jpegColorspace;
    const TJPF pf = TJPF_BGRX;

    if (tjDecompressHeader3(_jpegDecompressor
                            , data
                            , length
                            , &width
                            , &height
                            , &jpegSubsamp
                            , &jpegColorspace) == 0) {
        buffer.resize(static_cast<size_t>(width * height * tjPixelSize[pf]));
        if (tjDecompress2(_jpegDecompressor
                          , data
                          , length
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
           if (counter == 0) counter++;
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
