#ifndef ZMSQAPPLICATION_H
#define ZMSQAPPLICATION_H

#include <QApplication>

class QThread;
class CamController;
class NetCam;

class ZMSQApplication : public QApplication {
    Q_OBJECT
    Q_DISABLE_COPY(ZMSQApplication)

public:
   ZMSQApplication(int &argc, char **argv, QThread*, CamController*);
   ~ZMSQApplication();
   explicit ZMSQApplication();
   int exec();
   NetCam* startNetCam(const QString&);
private:
   QThread* networkThread;
   CamController* camCtrl;
private slots:
   void cleanup();
};

#endif // ZMSQAPPLICATION_H
