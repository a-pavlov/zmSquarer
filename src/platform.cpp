#include "platform.h"
#include <QtQml/qqml.h>

void Platform::registerQmlType() {
    qmlRegisterType<Platform>(
        "Platform", 0, 1, "Platform" );
}

Platform::Platform(QObject *parent) : QObject(parent) {}

QString Platform::getName() const {
#ifdef Q_OS_WIN
    return "windows"
#endif

#ifdef Q_OS_ANDROID
    return "android";
#endif

#ifdef Q_OS_LINUX
    return "linux";
#endif
}
