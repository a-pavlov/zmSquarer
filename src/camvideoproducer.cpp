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

BufferHolder::~BufferHolder() {
    //qDebug() << "close buffer size " << buffer.size();
}

void BufferHolder::allocate(unsigned int sz) {
    if (buffer.empty()) buffer.resize(sz);
}

unsigned char* BufferHolder::data() {
    return &buffer[0];
}

void CamVideoProducer::registerQmlType() {
    qmlRegisterType<CamVideoProducer>(
        "CamVideoProducer", 0, 1, "CamVideoProducer" );
}

CamVideoProducer::CamVideoProducer(QObject *parent )
    : QObject( parent )
    , _surface( nullptr )
    , netCam(nullptr)
    , bufferHolder(new BufferHolder())
    , errorOnCam(false)
    , camDisconnected(true) {
    timerId = startTimer( 1000 / 10 ); //15 fps
    counter = 0;
#ifdef WITH_TURBOJPEG
    _jpegDecompressor = tjInitDecompress();
#endif
}

CamVideoProducer::~CamVideoProducer() {
    //qDebug() << "cam video producer close";
    if (timerId != -1) {
        killTimer(timerId);
    }

    closeSurface();
    _surface = nullptr;

#ifdef WITH_TURBOJPEG
    tjDestroy(_jpegDecompressor);
#endif

    if (netCam) {
        netCam->deleteLater();
    }

    bufferHolder->deleteLater();
}

QAbstractVideoSurface* CamVideoProducer::videoSurface() const {
    return _surface;
}

int CamVideoProducer::monId() const {
    return monitorId;
}

int CamVideoProducer::setMonId(int monId) {    
    auto app = static_cast<ZMSQApplication*>(QApplication::instance());
    netCam = app->getCamController().createCam(monId);

    connect(netCam, SIGNAL(error()), this, SLOT(onError()));

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

    monitorId = monId;
    return monitorId;
}

void CamVideoProducer::setVideoSurface( QAbstractVideoSurface* s )
{
    closeSurface();
    _surface = s;
}

void CamVideoProducer::closeSurface()
{
    if( _surface && _surface->isActive() ) {
        //qDebug() << "stop surface";
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
            if (netCam != nullptr) {
                ZMSQApplication* app = static_cast<ZMSQApplication*>(QApplication::instance());
                app->getCamController().startCam(netCam);
            }
        }
    } else {
        if (timerId != -1) {
            //qDebug() << "stop timer " << timerId;
            killTimer(timerId);
            timerId = -1;
        }
    }
}

void CamVideoProducer::timerEvent( QTimerEvent* ) {
    if( !_surface )
        return;

    if (netCam == nullptr) {
        drawNoSignal();
        return;
    }

    QSharedPointer<RBuffer> ptr = netCam->getImageBuffer();

    // check we have the new buffer
    if (ptr.isNull() || (ptr->getContentLength() == 0 && ptr->getSoiPos() == 0)) {
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

        bufferHolder->allocate(static_cast<unsigned int>(width * height * tjPixelSize[pf]));

        if (tjDecompress2(_jpegDecompressor
                          , data
                          , length
                          , bufferHolder->data()
                          , width
                          , 0/*pitch*/
                          , height
                          , pf
                          , TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == 0) {
           QImage screenImage(bufferHolder->data(), width, height, tjPixelSize[pf]*width, QImage::Format_RGB32);
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
        Q_UNUSED(err)
        //qDebug() << "unable to decompress header " << err;
    }
#else
    QImage screenImage; //("/home/inkpot/dev/" + frames.at(currentFrame++ % frames.size()));

    bool res = screenImage.loadFromData(data, length, "JPG");
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
            if (counter == 0) ++counter;
        }

        //передадим полученный кадр на отрисовку
        _surface->present( QVideoFrame( screenImage ) );
    }
#endif
}

void CamVideoProducer::onError() {
    drawNoSignal();
}
