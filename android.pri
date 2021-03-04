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
