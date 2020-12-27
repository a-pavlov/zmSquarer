#ifndef CAMVIDEOPRODUCER_H
#define CAMVIDEOPRODUCER_H

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

#ifdef WITH_TURBOJPEG
extern "C" {
    #include <turbojpeg.h>
}
#endif

class NetCam;

class CamVideoProducer : public QObject {
    Q_OBJECT
public:
    static void registerQmlType();

    explicit CamVideoProducer(QObject *parent = nullptr);
    ~CamVideoProducer();

    Q_PROPERTY( QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface )
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface( QAbstractVideoSurface* s );

    QString url() const;
    QString setUrl(const QString&);
    void drawNoSignal();
protected:
    void timerEvent( QTimerEvent* );

private:
    void closeSurface();

private:
    int currentFrame;
    QAbstractVideoSurface* _surface;
    QVideoSurfaceFormat _format;
    QStringList frames;
    std::string urlCurrent;
    NetCam* netCam;
    std::vector<unsigned char> buffer;
    int counter;
#ifdef WITH_TURBOJPEG
    tjhandle _jpegDecompressor;
#endif
signals:
    void urlChanged();
};


#endif // CAMVIDEOPRODUCER_H
