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

class BufferHolder : public QObject {
    Q_OBJECT
public:
    ~BufferHolder();
    void allocate(unsigned int);
    unsigned char* data();
private:
    std::vector<unsigned char> buffer;
};

class CamVideoProducer : public QObject {
    Q_OBJECT
public:
    static void registerQmlType();

    explicit CamVideoProducer(QObject *parent = nullptr);
    ~CamVideoProducer();

    Q_PROPERTY( QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface )
    Q_PROPERTY(int monId READ monId WRITE setMonId NOTIFY monIdChanged)

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface( QAbstractVideoSurface* s );

    int monId() const;
    int setMonId(int);

    void drawNoSignal();
    Q_INVOKABLE void updateTimer(bool start);
protected:
    void timerEvent( QTimerEvent* );

private:
    void closeSurface();

private:
    int currentFrame;
    QAbstractVideoSurface* _surface;
    QVideoSurfaceFormat _format;
    QStringList frames;
    int monitorId;
    NetCam* netCam;
    BufferHolder* bufferHolder;
    int counter;
    bool errorOnCam;
    bool camDisconnected;
    int timerId;
#ifdef WITH_TURBOJPEG
    tjhandle _jpegDecompressor;
#endif
signals:
    void monIdChanged();
private slots:
    void onError();
};


#endif // CAMVIDEOPRODUCER_H
