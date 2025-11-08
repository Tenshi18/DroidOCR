**[EN]** ● **[[RU](README_ru.md)]**

# DroidOCR
Native Android application for recognizing Russian, Belarusian, Ukrainian, and English text using PaddlePaddle OCR v5 and ncnn.

## Features

- Russian, Belarusian, Ukrainian, and English text recognition
- Uses [PaddlePaddle/eslav_PP-OCRv5_mobile_rec](https://huggingface.co/PaddlePaddle/eslav_PP-OCRv5_mobile_rec) model
- Runs on ncnn (fast inference on CPU/GPU)
- Modern UI with Jetpack Compose and Material 3
- Material 3 dynamic colors (Android 12+)
- Dark theme support with automatic system theme detection
- Multi-language UI (English, Russian) with system settings integration
- Multi-line text detection and recognition
- Interactive text selection on images

## Requirements

- Android Studio Arctic Fox or newer
- Android SDK 24+ (Android 7.0+)
- NDK 25 or newer
- CMake 3.22.1+

## Building

Native libraries and models are already included. If something's missing, download it:

```bash
# ncnn
cd app/src/main/jniLibs
wget https://github.com/Tencent/ncnn/releases/download/20250916/ncnn-20250916-android-vulkan.zip
unzip ncnn-20250916-android-vulkan.zip
mv ncnn-20250916-android-vulkan ncnn

# OpenCV Mobile
wget https://github.com/nihui/opencv-mobile/releases/download/v4.12.0/opencv-mobile-4.12.0-android.zip
unzip opencv-mobile-4.12.0-android.zip
```

Or use the scripts:
```bash
./download_ncnn.sh
./download_opencv.sh
```

Then build:
```bash
./gradlew assembleDebug
```

## Project Structure

```
DroidOCR/
├── app/
│   ├── src/main/
│   │   ├── assets/                    # Resources
│   │   │   ├── PP_OCRv5_mobile_det.ncnn.param
│   │   │   ├── PP_OCRv5_mobile_det.ncnn.bin
│   │   │   ├── eslav_ppocrv5_rec.ncnn.param
│   │   │   ├── eslav_ppocrv5_rec.ncnn.bin
│   │   │   └── ppocrv5_eslav_dict.txt # Character dictionary
│   │   ├── cpp/                       # C++ code
│   │   │   ├── CMakeLists.txt
│   │   │   ├── droidocr_jni_full.cpp  # JNI bridge
│   │   │   ├── ppocrv5_full.h
│   │   │   └── ppocrv5_full.cpp       # OCR engine
│   │   ├── java/com/tenshi18/droidocr/
│   │   │   ├── MainActivity.kt        # Compose UI
│   │   │   ├── PPOCRv5Rec.kt         # Kotlin wrapper
│   │   │   ├── LanguageManager.kt    # OCR language management
│   │   │   ├── LocaleHelper.kt       # UI locale management
│   │   │   ├── OcrLanguage.kt       # OCR language enum
│   │   │   └── ui/theme/            # Material 3 theme
│   │   │       ├── Theme.kt         # Theme composable
│   │   │       ├── Color.kt        # Color schemes
│   │   │       ├── Type.kt         # Typography
│   │   │       └── ThemeMode.kt    # Theme mode enum
│   │   └── jniLibs/                   # Native libraries
│   │       ├── ncnn/
│   │       └── opencv-mobile-4.12.0-android/
│   └── build.gradle.kts
└── README.md
```

## Usage

1. Launch the app
2. Tap "Select Image"
3. Pick a photo with text
4. Tap the image to select text directly (fullscreen mode)

### UI Language

Change interface language in Android settings:
- Settings → Apps → DroidOCR → Language

## Supported Languages

- Russian (Cyrillic)
- Belarusian (Cyrillic)
- Ukrainian (Cyrillic)
- English (Latin)
- Numbers and special characters
- Mathematical symbols

## Technical Details

**Models:**
- Detection: PPOCRv5 Mobile
- Recognition: PPOCRv5 (eslav variant)
- Input: 48px height, dynamic width
- Dictionary: 518 characters
- Architecture: MobileNetV3 + BiLSTM + CTC

**Performance:**
- Speed: ~50-200ms per image (device-dependent)
- Model size: ~14 MB total
- Memory: ~50 MB minimum

## Dependencies

**Native:**
- [ncnn](https://github.com/Tencent/ncnn) (BSD 3-Clause) - neural network inference
- [OpenCV Mobile](https://github.com/nihui/opencv-mobile) (Apache 2.0) - image processing

**Android:**
- Jetpack Compose + Material 3
- AndroidX Preference
- Kotlin Coroutines

**Models:**
- [PaddlePaddle/eslav_PP-OCRv5_mobile_rec](https://huggingface.co/PaddlePaddle/eslav_PP-OCRv5_mobile_rec) (Apache 2.0)
- [PaddleOCR Repo](https://github.com/paddlepaddle/paddleocr) (Apache 2.0)

## License

Apache License 2.0

**Third-party:**
- ncnn: BSD 3-Clause - https://github.com/Tencent/ncnn
- OpenCV Mobile: Apache 2.0 - https://github.com/nihui/opencv-mobile
- PPOCRv5 models: Apache 2.0 - https://github.com/PaddlePaddle/PaddleOCR

All included in the repo.

## TODO

- [ ] **Shared Intent support**
- [ ] **Multilingual support**
- [ ] **Batch processing** - multiple images at once
- [ ] **Camera integration** - live OCR
- [ ] **History** - save recognized texts
- [ ] **CI/CD workflows** - GitHub Actions for automated builds and tests

## Credits

- [ncnn](https://github.com/Tencent/ncnn) - inference framework
- [OpenCV](https://github.com/opencv/opencv) - image processing
- [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR) - PPOCRv5 models
- [nihui/ncnn-android-ppocrv5](https://github.com/nihui/ncnn-android-ppocrv5) - Android example
