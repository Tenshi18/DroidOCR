#!/bin/bash

set -e

OPENCV_VERSION="4.12.0"
OPENCV_URL="https://github.com/nihui/opencv-mobile/releases/download/v34/opencv-mobile-${OPENCV_VERSION}-android.zip"
TARGET_DIR="app/src/main/jniLibs"

echo "Downloading opencv-mobile ${OPENCV_VERSION} for Android..."

mkdir -p ${TARGET_DIR}
cd ${TARGET_DIR}

if [ -d "opencv-mobile-${OPENCV_VERSION}-android" ]; then
    echo "opencv-mobile already exists in ${TARGET_DIR}/opencv-mobile-${OPENCV_VERSION}-android"
    echo "If you want to re-download, delete the directory first"
    exit 0
fi

echo "Downloading from ${OPENCV_URL}..."
wget -O opencv-mobile.zip ${OPENCV_URL}

echo "Extracting..."
unzip -q opencv-mobile.zip
rm opencv-mobile.zip

echo "opencv-mobile successfully downloaded and extracted to ${TARGET_DIR}/opencv-mobile-${OPENCV_VERSION}-android"
echo ""
echo "Directory structure:"
ls -la opencv-mobile-${OPENCV_VERSION}-android/

echo ""
echo "You can now build the project with: ./gradlew assembleDebug"

