#!/bin/bash

set -e

NCNN_VERSION="20250916"
NCNN_URL="https://github.com/Tencent/ncnn/releases/download/${NCNN_VERSION}/ncnn-${NCNN_VERSION}-android-vulkan.zip"
TARGET_DIR="app/src/main/jniLibs"

echo "Downloading ncnn ${NCNN_VERSION} for Android..."

mkdir -p ${TARGET_DIR}
cd ${TARGET_DIR}

if [ -d "ncnn" ]; then
    echo "ncnn already exists in ${TARGET_DIR}/ncnn"
    echo "If you want to re-download, delete the directory first: rm -rf ${TARGET_DIR}/ncnn"
    exit 0
fi

echo "Downloading from ${NCNN_URL}..."
wget -O ncnn.zip ${NCNN_URL}

echo "Extracting..."
unzip -q ncnn.zip
rm ncnn.zip

mv ncnn-${NCNN_VERSION}-android-vulkan ncnn

echo "ncnn successfully downloaded and extracted to ${TARGET_DIR}/ncnn"
echo ""
echo "Directory structure:"
ls -la ncnn/

echo ""
echo "You can now build the project with: ./gradlew assembleDebug"

