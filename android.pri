message(Android build.)

DISTFILES += \
    platform/android/AndroidManifest.xml \
    platform/android/build.gradle \
    platform/android/gradle.properties \
    platform/android/gradle/wrapper/gradle-wrapper.jar \
    platform/android/gradle/wrapper/gradle-wrapper.properties \
    platform/android/gradlew \
    platform/android/gradlew.bat \
    platform/android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/platform/android
DISTFILES += $$ANDROID_PACKAGE_SOURCE_DIR/src/org/dkfsoft/zmSquarer/ZMSquarerActivity.java

!isEmpty(TJPEG_ROOT) {
    LIBS += -L$$(TJPEG_ROOT)/build -lturbojpeg
    INCLUDEPATH += $$(TJPEG_ROOT)
}

#####################################################################
#   OpenSSL for Android                                             #
#   https://doc.qt.io/qt-5/android-openssl-support.html             #
#   https://github.com/KDAB/android_openssl                         #
#   comment it if no OpenSSL support is required                    #
#   some modifications are possibly required like:                  #
#   set ssl_versions to NDK root ~/android/ndk/21.3.6528147         #
#   qt_versions to 1.1 qt_architectures to armeabi-v7a explicitly   #
#   run build_ssl.sh                                                #
include(/home/inkpot/dev/android_openssl/openssl.pri)
#####################################################################
