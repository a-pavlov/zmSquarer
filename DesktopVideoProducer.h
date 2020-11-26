#pragma once

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>


struct context;

class DesktopVideoProducer : public QObject
{
    Q_OBJECT
public:
    static void registerQmlType();

    explicit DesktopVideoProducer(QObject *parent = 0 );
    ~DesktopVideoProducer();

    Q_PROPERTY( QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface )
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface( QAbstractVideoSurface* s );    

    QString url() const;
    QString setUrl(const QString&);
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
    context* ctx;
signals:
    void urlChanged();
};
