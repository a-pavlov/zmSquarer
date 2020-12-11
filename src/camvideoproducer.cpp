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


void CamVideoProducer::registerQmlType() {
    qmlRegisterType<CamVideoProducer>(
        "CamVideoProducer", 0, 1, "CamVideoProducer" );
}

CamVideoProducer::CamVideoProducer(QObject *parent )
    : QObject( parent ), _surface( 0 ), netCam(nullptr) {
    startTimer( 1000 / 15 ); //15 fps
}

CamVideoProducer::~CamVideoProducer() {
    closeSurface();
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
    netCam = app->startNetCam(u);
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

    if (ptr.isNull()) {
        qDebug() << "output buffer is null";
        return;
    }

    //Получим screenshot и преобразуем в формат подходящий для QVideoFrame
    //QPixmap screenPixmap = screen->grabWindow( desktop->screen()->winId() );
    //QImage screenImage("/home/inkpot/dev/QmlVideoProducer/test.jpg");//screenPixmap.toImage();
    //qDebug() << frames.at(currentFrame++ % frames.size());


    unsigned char* data = (unsigned char*)ptr->getPtr();
    int length = ptr->getContentLength();
    char *ptr_buffer;

    ptr_buffer = (char*)memmem(ptr->getPtr(), ptr->getContentLength(), "\xff\xd8", 2);

    if (ptr_buffer != NULL) {
        size_t soi_position = 0;

        soi_position = ptr_buffer - ptr->getPtr();

        if (soi_position > 0) {
            data = (unsigned char*)ptr->getPtr() + soi_position;
            length -= soi_position;
            qDebug() << "correction to " << soi_position;

            //memmove(netcam->receiving->ptr, netcam->receiving->ptr + soi_position,
            //        netcam->receiving->used - soi_position);
            //netcam->receiving->used -= soi_position;
        }

        // if (debug_level > CAMERA_INFO)
        //    motion_log(LOG_INFO, 0, "SOI found , position %d",
        //               __FUNCTION__, soi_position);
    }


    qDebug() << "image size " << length;
    QImage screenImage; //("/home/inkpot/dev/" + frames.at(currentFrame++ % frames.size()));
    bool res = screenImage.loadFromData(data, length, "JPG");


    if (res) {
        qDebug() << "image loaded";
    } else {
        qDebug() << "error on image loading";
    }

    if (screenImage.isNull()) {
        qDebug() << "null";
    }
    QVideoFrame::PixelFormat pixelFormat =
        QVideoFrame::pixelFormatFromImageFormat( screenImage.format() );

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
