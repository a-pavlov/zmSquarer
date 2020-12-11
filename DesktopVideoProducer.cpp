#include "DesktopVideoProducer.h"

#include <QtQml/qqml.h>

#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QDebug>
#include <QDir>
#include "zmsqapplication.h"

extern "C" {
#include "motion.h"
#include "netcam.h"
#include "logger.h"
#include "translate.h"
}


void DesktopVideoProducer::registerQmlType()
{
    // @uri DesktopVideoProducer
    qmlRegisterType<DesktopVideoProducer>(
        "DesktopVideoProducer", 0, 1,
        "DesktopVideoProducer" );
}

DesktopVideoProducer::DesktopVideoProducer(QObject *parent )
    : QObject( parent ), _surface( 0 )
{
    currentFrame = 0;
    QDir dir("/home/inkpot/dev");
    QStringList l1 = dir.entryList(QDir::Files | QDir::Readable);
    qDebug() << "dir size " << l1.size();
    std::copy_if(l1.begin(), l1.end(), std::back_inserter(frames), [&](QString str) { return str.endsWith(".jpg");});
    qDebug() << "frames " << frames.size();
    startTimer( 1000 / 15 ); //15 fps
    ctx = nullptr;
}

DesktopVideoProducer::~DesktopVideoProducer()
{
    closeSurface();
    if (ctx != nullptr) {
        qDebug() << "stop cam";
        ctx->event_stop = 1;
        //context_destroy(ctx);
    }
}

QAbstractVideoSurface* DesktopVideoProducer::videoSurface() const
{
    return _surface;
}

QString DesktopVideoProducer::url() const {
    return QString(urlCurrent.c_str());
}

QString DesktopVideoProducer::setUrl(const QString& u) {
    ZMSQApplication* app = static_cast<ZMSQApplication*>(QApplication::instance());
    qDebug() << "video producer set url " << u << " thread ";
    app->startNetCam(u);
    urlCurrent = u.toStdString();
    if (ctx != nullptr) {
        ctx->event_stop = 1;
    }

    ctx = (context*)malloc(sizeof(context));
    context_init(ctx);
    ctx->conf.netcam_url = urlCurrent.c_str();
    int minit = motion_init(ctx);
    int ncRet = netcam_start(ctx);
    qDebug() << "motion init " << minit << " netcam init " << ncRet;
    return u;
}

void DesktopVideoProducer::setVideoSurface( QAbstractVideoSurface* s )
{
    closeSurface();
    _surface = s;
}

void DesktopVideoProducer::closeSurface()
{
    if( _surface && _surface->isActive() )
        _surface->stop();
}

void DesktopVideoProducer::timerEvent( QTimerEvent* )
{
    if( !_surface )
        return;

    QScreen* screen = QGuiApplication::primaryScreen();
    QDesktopWidget* desktop = QApplication::desktop();

    if( !screen || !desktop )
        return;

    struct netcam_context* netcam = ctx->netcam;

    netcam_buff_ptr buff;

    /*
     * First we check whether a new image has arrived.  If not, we
     * setup to wait for 1/2 a frame time.  This will (hopefully)
     * help in synchronizing the camera frames with the motion main
     * loop.
     */
    pthread_mutex_lock(&netcam->mutex);

    if (netcam->imgcnt_last == netcam->imgcnt) {    /* Need to wait */
        struct timespec waittime;
        struct timeval curtime;
        int retcode;

        /*
         * We calculate the delay time (representing the desired frame
         * rate).  This delay time is in *nanoseconds*.
         * We will wait 0.5 seconds which gives a practical minimum
         * framerate of 2 which is desired for the motion_loop to
         * function.
         */
        gettimeofday(&curtime, NULL);
        curtime.tv_usec += 500000;

        if (curtime.tv_usec > 1000000) {
            curtime.tv_usec -= 1000000;
            curtime.tv_sec++;
        }

        waittime.tv_sec = curtime.tv_sec;
        waittime.tv_nsec = 1000L * curtime.tv_usec;

        do {
            retcode = pthread_cond_timedwait(&netcam->pic_ready,
                                             &netcam->mutex, &waittime);
        } while (retcode == EINTR);

        if (retcode) {    /* We assume a non-zero reply is ETIMEOUT */
            pthread_mutex_unlock(&netcam->mutex);

            MOTION_LOG(WRN, TYPE_NETCAM, NO_ERRNO
                ,_("no new pic, no signal rcvd"));

            //return NETCAM_GENERAL_ERROR | NETCAM_NOTHING_NEW_ERROR;
            qDebug() << "error nothing new";
        }

        MOTION_LOG(DBG, TYPE_NETCAM, NO_ERRNO,_("***new pic delay successful***"));
    }

    netcam->imgcnt_last = netcam->imgcnt;

    /* Set latest buffer as "current". */
    buff = netcam->latest;
    netcam->latest = netcam->jpegbuf;
    netcam->jpegbuf = buff;
    pthread_mutex_unlock(&netcam->mutex);

    /* Clear any error flag from previous work. */
    netcam->jpeg_error = 0;

    buff = netcam->jpegbuf;

    //Получим screenshot и преобразуем в формат подходящий для QVideoFrame
    //QPixmap screenPixmap = screen->grabWindow( desktop->screen()->winId() );
    //QImage screenImage("/home/inkpot/dev/QmlVideoProducer/test.jpg");//screenPixmap.toImage();
    //qDebug() << frames.at(currentFrame++ % frames.size());
    QImage screenImage; //("/home/inkpot/dev/" + frames.at(currentFrame++ % frames.size()));
    bool res = screenImage.loadFromData((unsigned char*)buff->ptr, buff->used, "JPG");


    if (res) {
        //qDebug() << "image loaded";
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
