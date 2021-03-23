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
    contains(ANDROID_TARGET_ARCH, armeabi-v7a) {
        #ANDROID_EXTRA_LIBS += \
        #    $$(TJPEG_ROOT)/arm/libjpeg.so
        LIBS += -L$$(TJPEG_ROOT)/arm -lturbojpeg
    }

    contains(ANDROID_TARGET_ARCH, arm64-v8a) {
        #ANDROID_EXTRA_LIBS += \
        #    $$(TJPEG_ROOT)/arm64/libjpeg.so
        LIBS += -L$$(TJPEG_ROOT)/arm64 -lturbojpeg
    }

    contains(ANDROID_TARGET_ARCH, x86) {
        #ANDROID_EXTRA_LIBS += \
        #    $$(TJPEG_ROOT)/x86/libjpeg.so
        LIBS += -L$$(TJPEG_ROOT)/x86 -lturbojpeg
    }

    contains(ANDROID_TARGET_ARCH, x86_64) {
        #ANDROID_EXTRA_LIBS += \
        #    $$(TJPEG_ROOT)/x86_64/libjpeg.so
        LIBS += -L$$(TJPEG_ROOT)/x86_64 -lturbojpeg
    }

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
