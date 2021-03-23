#!/bin/sh

# Set these variables to suit your needs
NDK_PATH="/home/inkpot/dev/android/ndk-bundle"
TOOLCHAIN="clang"
ANDROID_VERSION=21

mkdir -p build
cd build
cmake -G"Unix Makefiles" \
    -DANDROID_ABI=armeabi-v7a \
    -DANDROID_ARM_MODE=arm \
    -DANDROID_PLATFORM=android-${ANDROID_VERSION} \
    -DANDROID_TOOLCHAIN=${TOOLCHAIN} \
    -DCMAKE_ASM_FLAGS="--target=aarch64-linux-android${ANDROID_VERSION}" \
    -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
    ..
make

if [ $? -eq 0 ]
then
    echo "ARM build successfull."
    cd ..
    mkdir -p arm
    cp build/libjpeg.so arm/
    cp build/libturbojpeg.a arm/
    rm -rf build
else
  exit 1
fi


mkdir -p build
cd build
cmake -G"Unix Makefiles" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_ARM_MODE=arm \
    -DANDROID_PLATFORM=android-${ANDROID_VERSION} \
    -DANDROID_TOOLCHAIN=${TOOLCHAIN} \
    -DCMAKE_ASM_FLAGS="--target=aarch64-linux-android${ANDROID_VERSION}" \
    -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
    ..
make

if [ $? -eq 0 ]
then
    echo "ARM 64 build successfull."
    cd ..
    mkdir -p arm64
    cp build/libjpeg.so arm64/
    cp build/libturbojpeg.a arm64/
    rm -rf build
else
  exit 1
fi


mkdir -p build
cd build

cmake -G"Unix Makefiles" \
    -DANDROID_ABI=x86 \
    -DANDROID_PLATFORM=android-${ANDROID_VERSION} \
    -DANDROID_TOOLCHAIN=${TOOLCHAIN} \
    -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
    ..
make

if [ $? -eq 0 ]
then
    echo "x86 build successfull."
    cd ..
    mkdir -p x86
    cp build/libjpeg.so x86/
    cp build/libturbojpeg.a x86/
    rm -rf build
else
  exit 1
fi

mkdir -p build
cd build

cmake -G"Unix Makefiles" \
    -DANDROID_ABI=x86_64 \
    -DANDROID_PLATFORM=android-${ANDROID_VERSION} \
    -DANDROID_TOOLCHAIN=${TOOLCHAIN} \
    -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
    ..
make

if [ $? -eq 0 ]
then
    echo "x86_64 build successfull."
    cd ..
    mkdir -p x86_64
    cp build/libjpeg.so x86_64/
    cp build/libturbojpeg.a x86_64/
    rm -rf build
else
  exit 1
fi

echo "Done."