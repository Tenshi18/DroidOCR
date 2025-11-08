**[[EN](README.md)]** ● **[RU]**

# DroidOCR
Нативное Android-приложение для распознавания русского, белорусского, украинского и английского текста на базе PaddlePaddle OCR v5 и ncnn.

## Особенности

- Распознавание русского, белорусского, украинского и английского текста
- Использование модели [PaddlePaddle/eslav_PP-OCRv5_mobile_rec](https://huggingface.co/PaddlePaddle/eslav_PP-OCRv5_mobile_rec)
- Работа на ncnn (быстрый inference на CPU/GPU)
- Современный UI на Jetpack Compose и Material 3
- Динамические цвета Material 3 (Android 12+)
- Поддержка темной темы с автоматическим определением системной темы
- Многоязычный интерфейс (английский, русский) с интеграцией в системные настройки
- Распознавание многострочного текста
- Интерактивное выделение текста на изображениях

## Требования

- Android Studio Arctic Fox или новее
- Android SDK 24+ (Android 7.0+)
- NDK 25 или новее
- CMake 3.22.1+

## Сборка

Нативные библиотеки и модели уже включены. Если чего-то не хватает, скачайте:

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

Или используйте скрипты:
```bash
./download_ncnn.sh
./download_opencv.sh
```

Сборка:
```bash
./gradlew assembleDebug
```

## Структура проекта

```
DroidOCR/
├── app/
│   ├── src/main/
│   │   ├── assets/                    # Ресурсы
│   │   │   ├── PP_OCRv5_mobile_det.ncnn.param
│   │   │   ├── PP_OCRv5_mobile_det.ncnn.bin
│   │   │   ├── eslav_ppocrv5_rec.ncnn.param
│   │   │   ├── eslav_ppocrv5_rec.ncnn.bin
│   │   │   └── ppocrv5_eslav_dict.txt # Словарь символов
│   │   ├── cpp/                       # C++ код
│   │   │   ├── CMakeLists.txt
│   │   │   ├── droidocr_jni_full.cpp  # JNI мост
│   │   │   ├── ppocrv5_full.h
│   │   │   └── ppocrv5_full.cpp       # OCR engine
│   │   ├── java/com/tenshi18/droidocr/
│   │   │   ├── MainActivity.kt        # UI на Compose
│   │   │   ├── PPOCRv5Rec.kt         # Kotlin wrapper
│   │   │   ├── LanguageManager.kt    # Управление языками OCR
│   │   │   ├── LocaleHelper.kt       # Управление локалью UI
│   │   │   ├── OcrLanguage.kt       # Enum языков OCR
│   │   │   └── ui/theme/            # Material 3 тема
│   │   │       ├── Theme.kt         # Composable темы
│   │   │       ├── Color.kt        # Цветовые схемы
│   │   │       ├── Type.kt         # Типографика
│   │   │       └── ThemeMode.kt    # Enum режима темы
│   │   └── jniLibs/                   # Нативные библиотеки
│   │       ├── ncnn/
│   │       └── opencv-mobile-4.12.0-android/
│   └── build.gradle.kts
└── README.md
```

## Использование

1. Запустите приложение
2. Нажмите "Выбрать изображение"
3. Выберите фото с текстом
4. Нажмите на изображение, чтобы выделить текст прямо на нём (полноэкранный режим)

### Язык интерфейса

Язык интерфейса меняется в системных настройках Android:
- Настройки → Приложения → DroidOCR → Язык

## Поддерживаемые языки

- Русский (кириллица)
- Белорусский (кириллица)
- Украинский (кириллица)
- Английский (латиница)
- Цифры и специальные символы
- Математические символы

## Технические детали

**Модели:**
- Детекция: PPOCRv5 Mobile
- Распознавание: PPOCRv5 (версия для восточнославянских языков, `eslav`)
- Входной размер: высота 48px, динамическая ширина
- Словарь: 518 символов
- Архитектура: MobileNetV3 + BiLSTM + CTC

**Производительность:**
- Скорость: ~50-200ms на изображение (зависит от устройства)
- Размер моделей: ~14 MB суммарно
- Память: ~50 MB минимум

## Зависимости

**Нативные:**
- [ncnn](https://github.com/Tencent/ncnn) (BSD 3-Clause) - inference для нейросетей
- [OpenCV Mobile](https://github.com/nihui/opencv-mobile) (Apache 2.0) - обработка изображений

**Android:**
- Jetpack Compose + Material 3
- AndroidX Preference
- Kotlin Coroutines

**Модели:**
- [PaddlePaddle/eslav_PP-OCRv5_mobile_rec](https://huggingface.co/PaddlePaddle/eslav_PP-OCRv5_mobile_rec) (Apache 2.0)
- [Репозиторий PaddleOCR](https://github.com/paddlepaddle/paddleocr) (Apache 2.0)

## Лицензия

Apache License 2.0

**Сторонние:**
- ncnn: BSD 3-Clause - https://github.com/Tencent/ncnn
- OpenCV Mobile: Apache 2.0 - https://github.com/nihui/opencv-mobile
- Модели PPOCRv5: Apache 2.0 - https://github.com/PaddlePaddle/PaddleOCR

Всё включено в репозиторий.

## TODO

- [ ] **Поддержка Shared Intent**
- [ ] **Поддержка мультиязычного распознавания текста**
- [ ] **Пакетная обработка** - несколько изображений сразу
- [ ] **Интеграция камеры** - OCR в реальном времени
- [ ] **История** - сохранение распознанных текстов
- [ ] **CI/CD workflows** - GitHub Actions для автоматической сборки и тестирования

## Благодарности

- [ncnn](https://github.com/Tencent/ncnn) - inference-фреймворк
- [OpenCV](https://github.com/opencv/opencv) - обработка изображений
- [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR) - модели PPOCRv5
- [nihui/ncnn-android-ppocrv5](https://github.com/nihui/ncnn-android-ppocrv5) - пример Android-приложения
