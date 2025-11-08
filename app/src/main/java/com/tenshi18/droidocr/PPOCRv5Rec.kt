/*
 * Copyright 2025 DroidOCR Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tenshi18.droidocr

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.PointF

/**
 * Результат распознавания одного текстового региона
 */
data class TextRegion(
    val text: String,
    val corners: Array<PointF>,
    val confidence: Float
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false
        other as TextRegion
        return text == other.text && corners.contentEquals(other.corners) && confidence == other.confidence
    }

    override fun hashCode(): Int {
        var result = text.hashCode()
        result = 31 * result + corners.contentHashCode()
        result = 31 * result + confidence.hashCode()
        return result
    }
}

/**
 * JNI wrapper для работы с моделью распознавания текста PPOCRv5
 */
class PPOCRv5Rec {
    
    /**
     * Загружает модель распознавания из assets
     * @param assetManager AssetManager для доступа к assets
     * @param detParamPath путь к .param файлу detection модели
     * @param detBinPath путь к .bin файлу detection модели
     * @param recParamPath путь к .param файлу recognition модели
     * @param recBinPath путь к .bin файлу recognition модели
     * @param dictPath путь к файлу словаря
     * @param useGpu использовать ли GPU (Vulkan)
     * @return true если модель успешно загружена
     */
    external fun loadModel(
        assetManager: AssetManager,
        detParamPath: String,
        detBinPath: String,
        recParamPath: String,
        recBinPath: String,
        dictPath: String,
        useGpu: Boolean = false
    ): Boolean
    
    /**
     * Распознает текст на изображении (detection + recognition)
     * @param bitmap изображение для распознавания
     * @return распознанный текст (каждая строка с новой строки)
     */
    external fun detectAndRecognize(bitmap: Bitmap): String
    
    /**
     * Распознает текст на изображении и возвращает детальные результаты с координатами
     * @param bitmap изображение для распознавания
     * @return массив найденных текстовых регионов с координатами и текстом
     */
    external fun detectAndRecognizeWithBoxes(bitmap: Bitmap): Array<TextRegion>
    
    /**
     * Переключает язык распознавания
     * Освобождает текущую модель и загружает новую
     * @param assetManager AssetManager для доступа к assets
     * @param detParamPath путь к .param файлу detection модели
     * @param detBinPath путь к .bin файлу detection модели
     * @param recParamPath путь к .param файлу recognition модели
     * @param recBinPath путь к .bin файлу recognition модели
     * @param dictPath путь к файлу словаря
     * @param useGpu использовать ли GPU (Vulkan)
     * @return true если модель успешно загружена
     */
    external fun switchLanguage(
        assetManager: AssetManager,
        detParamPath: String,
        detBinPath: String,
        recParamPath: String,
        recBinPath: String,
        dictPath: String,
        useGpu: Boolean = false
    ): Boolean
    
    /**
     * Освобождает ресурсы модели
     */
    external fun release()
    
    companion object {
        init {
            System.loadLibrary("droidocr")
        }
    }
}

