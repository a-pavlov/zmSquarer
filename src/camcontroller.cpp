#include "camcontroller.h"
#include "netcam2.h"

CamController::CamController(QObject *parent) : QObject(parent)
{

}

void CamController::startCam(const QString& url) {
    pn = new NetCam("http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=1&scale=100&maxfps=30&buffer=1000&user=admin&pass=root");
    pn->start();
}

NetCam* CamController::getNetCam() {
    return pn;
}
