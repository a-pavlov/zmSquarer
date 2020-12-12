#ifndef ZMSQAPPLICATION_H
#define ZMSQAPPLICATION_H

#include <QApplication>
#include "camcontroller.h"

class ZMSQApplication : public QApplication {
    Q_OBJECT
    Q_DISABLE_COPY(ZMSQApplication)

public:
   ZMSQApplication(int &argc, char **argv);
   ~ZMSQApplication();
   explicit ZMSQApplication();
   int exec();
   CamController& getCamController() {
       return camCtrl;
   }
private:
   CamController camCtrl;
private slots:
   void cleanup();
};

#endif // ZMSQAPPLICATION_H
